#include "keyring.h"
#include <cstddef>
#include <expected>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "environment.h"
#ifdef _WIN32
#include <wincred.h>
#include <windows.h>
#include "string_manip.h"
#elifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <MacTypes.h>
#include <Security/Security.h>
#elifdef __linux__
#include <libsecret/secret.h>
#endif

#ifdef __APPLE__
namespace
{
    std::string str(CFStringRef value)
    {
        if (value == nullptr)
        {
            return {};
        }
        CFIndex size{ CFStringGetMaximumSizeForEncoding(CFStringGetLength(value), kCFStringEncodingUTF8) + 1 };
        std::string result(static_cast<std::size_t>(size), '\0');
        if (CFStringGetCString(value, result.data(), size, kCFStringEncodingUTF8) == 0U)
        {
            return {};
        }
        result.resize(result.find('\0'));
        return result;
    }
}
#elifdef __linux__
static const SecretSchema KEYRING_SCHEMA{ "org.nickvision.keyring",
                                          SECRET_SCHEMA_NONE,
                                          { { "application", SECRET_SCHEMA_ATTRIBUTE_STRING },
                                            { "name", SECRET_SCHEMA_ATTRIBUTE_STRING },
                                            { "url", SECRET_SCHEMA_ATTRIBUTE_STRING },
                                            { "username", SECRET_SCHEMA_ATTRIBUTE_STRING },
                                            { "NULL", SECRET_SCHEMA_ATTRIBUTE_STRING } } };
#endif

namespace parabolic::keyring
{
    Credential::Credential(std::string name, std::string url, std::string username, std::string password)
        : m_name{ std::move(name) },
          m_url{ std::move(url) },
          m_username{ std::move(username) },
          m_password{ std::move(password) }
    {
    }

    const std::string& Credential::name() const
    {
        return m_name;
    }

    void Credential::set_name(const std::string& name)
    {
        m_name = name;
    }

    const std::string& Credential::url() const
    {
        return m_url;
    }

    void Credential::set_url(const std::string& url)
    {
        m_url = url;
    }

    const std::string& Credential::username() const
    {
        return m_username;
    }

    void Credential::set_username(const std::string& username)
    {
        m_username = username;
    }

    const std::string& Credential::password() const
    {
        return m_password;
    }

    void Credential::set_password(const std::string& password)
    {
        m_password = password;
    }

    Store::Store(std::string_view app_id)
        : m_app_id{ app_id }
    {
    }

    std::expected<void, std::string> Store::add(const Credential& credential)
    {
        std::scoped_lock lock{ m_mutex };
        if (credential.name().empty())
        {
            return std::unexpected<std::string>("The credential name cannot be empty.");
        }
#ifdef _WIN32
        std::wstring target{ string_manip::wstr(m_app_id + "/" + credential.name()) };
        CREDENTIALW* existing{ nullptr };
        if (CredReadW(target.c_str(), CRED_TYPE_GENERIC, 0, &existing) != FALSE)
        {
            CredFree(existing);
            return std::unexpected<std::string>("A credential with this name already exists.");
        }
        std::wstring url{ string_manip::wstr(credential.url()) };
        std::wstring username{ string_manip::wstr(credential.username()) };
        std::wstring password{ string_manip::wstr(credential.password()) };
        CREDENTIALW cred{ .Type = CRED_TYPE_GENERIC,
                          .TargetName = target.data(),
                          .Comment = url.data(),
                          .CredentialBlobSize = static_cast<DWORD>(password.size() * sizeof(wchar_t)),
                          .CredentialBlob = reinterpret_cast<LPBYTE>(password.data()),
                          .Persist = CRED_PERSIST_LOCAL_MACHINE,
                          .UserName = username.data() };
        if (CredWriteW(&cred, 0) == FALSE)
        {
            return std::unexpected(environment::last_system_error());
        }
        return {};
#elifdef __APPLE__
        CFStringRef service_ref{ CFStringCreateWithCString(nullptr, m_app_id.c_str(), kCFStringEncodingUTF8) };
        CFStringRef name_ref{ CFStringCreateWithCString(nullptr, credential.name().c_str(), kCFStringEncodingUTF8) };
        CFStringRef url_ref{ CFStringCreateWithCString(nullptr, credential.url().c_str(), kCFStringEncodingUTF8) };
        CFDataRef username_data{ CFDataCreate(nullptr,
                                              reinterpret_cast<const UInt8*>(credential.username().data()),
                                              static_cast<CFIndex>(credential.username().size())) };
        CFDataRef password_data{ CFDataCreate(nullptr,
                                              reinterpret_cast<const UInt8*>(credential.password().data()),
                                              static_cast<CFIndex>(credential.password().size())) };
        CFMutableDictionaryRef query{
            CFDictionaryCreateMutable(nullptr, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks)
        };
        CFDictionaryAddValue(query, kSecClass, kSecClassGenericPassword);
        CFDictionaryAddValue(query, kSecAttrService, service_ref);
        CFDictionaryAddValue(query, kSecAttrAccount, name_ref);
        CFDictionaryAddValue(query, kSecAttrLabel, name_ref);
        CFDictionaryAddValue(query, kSecAttrComment, url_ref);
        CFDictionaryAddValue(query, kSecAttrGeneric, username_data);
        CFDictionaryAddValue(query, kSecValueData, password_data);
        OSStatus status{ SecItemAdd(query, nullptr) };
        CFRelease(query);
        CFRelease(password_data);
        CFRelease(username_data);
        CFRelease(url_ref);
        CFRelease(name_ref);
        CFRelease(service_ref);
        if (status == errSecDuplicateItem)
        {
            return std::unexpected<std::string>("A credential with this name already exists.");
        }
        if (status != errSecSuccess)
        {
            CFStringRef message_ref{ SecCopyErrorMessageString(status, nullptr) };
            std::string message{ str(message_ref) };
            if (message_ref != nullptr)
            {
                CFRelease(message_ref);
            }
            return std::unexpected(message);
        }
        return {};
#else
        GError* error{ nullptr };
        char* existing{ secret_password_lookup_sync(&KEYRING_SCHEMA,
                                                    nullptr,
                                                    &error,
                                                    "application",
                                                    m_app_id.c_str(),
                                                    "name",
                                                    credential.name().c_str(),
                                                    nullptr) };
        if (error)
        {
            std::string message{ error->message };
            g_error_free(error);
            return std::unexpected(message);
        }
        if (existing)
        {
            secret_password_free(existing);
            return std::unexpected<std::string>("A credential with this name already exists.");
        }
        secret_password_store_sync(&KEYRING_SCHEMA,
                                   SECRET_COLLECTION_DEFAULT,
                                   credential.name().c_str(),
                                   credential.password().c_str(),
                                   nullptr,
                                   &error,
                                   "application",
                                   m_app_id.c_str(),
                                   "name",
                                   credential.name().c_str(),
                                   "url",
                                   credential.url().c_str(),
                                   "username",
                                   credential.username().c_str(),
                                   nullptr);
        if (error)
        {
            std::string message{ error->message };
            g_error_free(error);
            return std::unexpected(message);
        }
        return {};
#endif
    }

    std::expected<Credential, std::string> Store::get(const std::string& name) const
    {
        std::scoped_lock lock{ m_mutex };
        if (name.empty())
        {
            return std::unexpected<std::string>("The credential name cannot be empty.");
        }
#ifdef _WIN32
        CREDENTIALW* item{ nullptr };
        if (CredReadW(string_manip::wstr(m_app_id + "/" + name).c_str(), CRED_TYPE_GENERIC, 0, &item) == FALSE)
        {
            return std::unexpected(environment::last_system_error());
        }
        std::wstring password{ reinterpret_cast<const wchar_t*>(item->CredentialBlob),
                               item->CredentialBlobSize / sizeof(wchar_t) };
        Credential credential{ name,
                               item->Comment ? string_manip::str(item->Comment) : std::string{},
                               item->UserName ? string_manip::str(item->UserName) : std::string{},
                               string_manip::str(password) };
        CredFree(item);
        return credential;
#elifdef __APPLE__
        CFStringRef service_ref{ CFStringCreateWithCString(nullptr, m_app_id.c_str(), kCFStringEncodingUTF8) };
        CFStringRef name_ref{ CFStringCreateWithCString(nullptr, name.c_str(), kCFStringEncodingUTF8) };
        CFMutableDictionaryRef query{
            CFDictionaryCreateMutable(nullptr, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks)
        };
        CFDictionaryAddValue(query, kSecClass, kSecClassGenericPassword);
        CFDictionaryAddValue(query, kSecAttrService, service_ref);
        CFDictionaryAddValue(query, kSecAttrAccount, name_ref);
        CFDictionaryAddValue(query, kSecMatchLimit, kSecMatchLimitOne);
        CFDictionaryAddValue(query, kSecReturnAttributes, kCFBooleanTrue);
        CFDictionaryAddValue(query, kSecReturnData, kCFBooleanTrue);
        CFTypeRef result{ nullptr };
        OSStatus status{ SecItemCopyMatching(query, &result) };
        CFRelease(query);
        CFRelease(name_ref);
        CFRelease(service_ref);
        if (status != errSecSuccess)
        {
            CFStringRef message_ref{ SecCopyErrorMessageString(status, nullptr) };
            std::string message{ str(message_ref) };
            if (message_ref != nullptr)
            {
                CFRelease(message_ref);
            }
            return std::unexpected(message);
        }
        if (result == nullptr)
        {
            return std::unexpected<std::string>("The credential could not be found.");
        }
        CFDictionaryRef item{ static_cast<CFDictionaryRef>(result) };
        CFDataRef username_data{ static_cast<CFDataRef>(CFDictionaryGetValue(item, kSecAttrGeneric)) };
        CFDataRef password_data{ static_cast<CFDataRef>(CFDictionaryGetValue(item, kSecValueData)) };
        Credential credential{ name,
                               str(static_cast<CFStringRef>(CFDictionaryGetValue(item, kSecAttrComment))),
                               username_data != nullptr
                                       ? std::string{ reinterpret_cast<const char*>(CFDataGetBytePtr(username_data)),
                                                      static_cast<std::size_t>(CFDataGetLength(username_data)) }
                                       : std::string{},
                               password_data != nullptr
                                       ? std::string{ reinterpret_cast<const char*>(CFDataGetBytePtr(password_data)),
                                                      static_cast<std::size_t>(CFDataGetLength(password_data)) }
                                       : std::string{} };
        CFRelease(result);
        return credential;
#else
        GHashTable* attributes{ g_hash_table_new(g_str_hash, g_str_equal) };
        g_hash_table_insert(attributes, const_cast<char*>("application"), const_cast<char*>(m_app_id.c_str()));
        g_hash_table_insert(attributes, const_cast<char*>("name"), const_cast<char*>(name.c_str()));
        GError* error{ nullptr };
        GList* items{ secret_service_search_sync(nullptr,
                                                 &KEYRING_SCHEMA,
                                                 attributes,
                                                 static_cast<SecretSearchFlags>(SECRET_SEARCH_UNLOCK |
                                                                                SECRET_SEARCH_LOAD_SECRETS),
                                                 nullptr,
                                                 &error) };
        g_hash_table_unref(attributes);
        if (error)
        {
            std::string message{ error->message };
            g_error_free(error);
            return std::unexpected(message);
        }
        if (!items)
        {
            return std::unexpected<std::string>("The credential could not be found.");
        }
        std::optional<Credential> credential;
        if (items)
        {
            SecretItem* item{ static_cast<SecretItem*>(items->data) };
            GHashTable* item_attributes{ secret_item_get_attributes(item) };
            SecretValue* value{ secret_item_get_secret(item) };
            const char* url{ static_cast<const char*>(g_hash_table_lookup(item_attributes, "url")) };
            const char* username{ static_cast<const char*>(g_hash_table_lookup(item_attributes, "username")) };
            const char* password{ value ? secret_value_get_text(value) : nullptr };
            credential = Credential{ name, url ? url : "", username ? username : "", password ? password : "" };
            if (value)
            {
                secret_value_unref(value);
            }
            g_hash_table_unref(item_attributes);
        }
        g_list_free_full(items, g_object_unref);
        return *credential;
#endif
    }

    std::vector<Credential> Store::get_all() const
    {
        std::scoped_lock lock{ m_mutex };
        std::vector<Credential> credentials;
#ifdef _WIN32
        DWORD count{ 0 };
        PCREDENTIALW* items{ nullptr };
        if (CredEnumerateW((string_manip::wstr(m_app_id + "/") + L"*").c_str(), 0, &count, &items) != FALSE)
        {
            for (DWORD i{ 0 }; i < count; i++)
            {
                std::wstring_view target{ items[i]->TargetName };
                credentials.emplace_back(string_manip::str(
                                                 std::wstring{ target.substr(string_manip::wstr(m_app_id).size() + 1) }),
                                         items[i]->Comment ? string_manip::str(items[i]->Comment) : std::string{},
                                         items[i]->UserName ? string_manip::str(items[i]->UserName) : std::string{},
                                         std::string{});
            }
            CredFree(items);
        }
#elifdef __APPLE__
        CFStringRef service_ref{ CFStringCreateWithCString(nullptr, m_app_id.c_str(), kCFStringEncodingUTF8) };
        CFMutableDictionaryRef query{
            CFDictionaryCreateMutable(nullptr, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks)
        };
        CFDictionaryAddValue(query, kSecClass, kSecClassGenericPassword);
        CFDictionaryAddValue(query, kSecAttrService, service_ref);
        CFDictionaryAddValue(query, kSecMatchLimit, kSecMatchLimitAll);
        CFDictionaryAddValue(query, kSecReturnAttributes, kCFBooleanTrue);
        CFTypeRef result{ nullptr };
        OSStatus status{ SecItemCopyMatching(query, &result) };
        CFRelease(query);
        if (status == errSecSuccess && result != nullptr)
        {
            CFArrayRef items{ static_cast<CFArrayRef>(result) };
            for (CFIndex i{ 0 }; i < CFArrayGetCount(items); i++)
            {
                CFDictionaryRef item{ static_cast<CFDictionaryRef>(CFArrayGetValueAtIndex(items, i)) };
                CFStringRef name_ref{ static_cast<CFStringRef>(CFDictionaryGetValue(item, kSecAttrAccount)) };
                CFDataRef username_data{ static_cast<CFDataRef>(CFDictionaryGetValue(item, kSecAttrGeneric)) };
                credentials.emplace_back(str(name_ref),
                                         str(static_cast<CFStringRef>(CFDictionaryGetValue(item, kSecAttrComment))),
                                         username_data != nullptr
                                                 ? std::string{ reinterpret_cast<const char*>(CFDataGetBytePtr(username_data)),
                                                                static_cast<std::size_t>(CFDataGetLength(username_data)) }
                                                 : std::string{},
                                         std::string{});
            }
            CFRelease(result);
        }
        CFRelease(service_ref);
#else
        GHashTable* attributes{ g_hash_table_new(g_str_hash, g_str_equal) };
        g_hash_table_insert(attributes, const_cast<char*>("application"), const_cast<char*>(m_app_id.c_str()));
        GError* error{ nullptr };
        GList* items{ secret_service_search_sync(nullptr,
                                                 &KEYRING_SCHEMA,
                                                 attributes,
                                                 static_cast<SecretSearchFlags>(SECRET_SEARCH_ALL | SECRET_SEARCH_UNLOCK),
                                                 nullptr,
                                                 &error) };
        g_hash_table_unref(attributes);
        if (error)
        {
            g_error_free(error);
        }
        else
        {
            for (GList* i{ items }; i; i = i->next)
            {
                SecretItem* item{ static_cast<SecretItem*>(i->data) };
                GHashTable* item_attributes{ secret_item_get_attributes(item) };
                const char* name{ static_cast<const char*>(g_hash_table_lookup(item_attributes, "name")) };
                const char* url{ static_cast<const char*>(g_hash_table_lookup(item_attributes, "url")) };
                const char* username{ static_cast<const char*>(g_hash_table_lookup(item_attributes, "username")) };
                credentials.emplace_back(name ? name : "", url ? url : "", username ? username : "", std::string{});
                g_hash_table_unref(item_attributes);
            }
            g_list_free_full(items, g_object_unref);
        }
#endif
        return credentials;
    }

    std::expected<void, std::string> Store::remove(const std::string& name)
    {
        std::scoped_lock lock{ m_mutex };
        if (name.empty())
        {
            return std::unexpected<std::string>("The credential name cannot be empty.");
        }
#ifdef _WIN32
        if (CredDeleteW(string_manip::wstr(m_app_id + "/" + name).c_str(), CRED_TYPE_GENERIC, 0) == FALSE)
        {
            return std::unexpected(environment::last_system_error());
        }
        return {};
#elifdef __APPLE__
        CFStringRef service_ref{ CFStringCreateWithCString(nullptr, m_app_id.c_str(), kCFStringEncodingUTF8) };
        CFStringRef name_ref{ CFStringCreateWithCString(nullptr, name.c_str(), kCFStringEncodingUTF8) };
        CFMutableDictionaryRef query{
            CFDictionaryCreateMutable(nullptr, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks)
        };
        CFDictionaryAddValue(query, kSecClass, kSecClassGenericPassword);
        CFDictionaryAddValue(query, kSecAttrService, service_ref);
        CFDictionaryAddValue(query, kSecAttrAccount, name_ref);
        OSStatus status{ SecItemDelete(query) };
        CFRelease(query);
        CFRelease(name_ref);
        CFRelease(service_ref);
        if (status != errSecSuccess)
        {
            CFStringRef message_ref{ SecCopyErrorMessageString(status, nullptr) };
            std::string message{ str(message_ref) };
            if (message_ref != nullptr)
            {
                CFRelease(message_ref);
            }
            return std::unexpected(message);
        }
        return {};
#else
        GError* error{ nullptr };
        bool removed{ secret_password_clear_sync(&KEYRING_SCHEMA,
                                                 nullptr,
                                                 &error,
                                                 "application",
                                                 m_app_id.c_str(),
                                                 "name",
                                                 name.c_str(),
                                                 nullptr) != FALSE };
        if (error)
        {
            std::string message{ error->message };
            g_error_free(error);
            return std::unexpected(message);
        }
        if (!removed)
        {
            return std::unexpected<std::string>("The credential could not be found.");
        }
        return {};
#endif
    }

    std::expected<void, std::string> Store::update(const Credential& credential)
    {
        std::scoped_lock lock{ m_mutex };
        if (credential.name().empty())
        {
            return std::unexpected<std::string>("The credential name cannot be empty.");
        }
#ifdef _WIN32
        std::wstring target{ string_manip::wstr(m_app_id + "/" + credential.name()) };
        CREDENTIALW* existing{ nullptr };
        if (CredReadW(target.c_str(), CRED_TYPE_GENERIC, 0, &existing) == FALSE)
        {
            return std::unexpected(environment::last_system_error());
        }
        CredFree(existing);
        std::wstring url{ string_manip::wstr(credential.url()) };
        std::wstring username{ string_manip::wstr(credential.username()) };
        std::wstring password{ string_manip::wstr(credential.password()) };
        CREDENTIALW cred{ .Type = CRED_TYPE_GENERIC,
                          .TargetName = target.data(),
                          .Comment = url.data(),
                          .CredentialBlobSize = static_cast<DWORD>(password.size() * sizeof(wchar_t)),
                          .CredentialBlob = reinterpret_cast<LPBYTE>(password.data()),
                          .Persist = CRED_PERSIST_LOCAL_MACHINE,
                          .UserName = username.data() };
        if (CredWriteW(&cred, 0) == FALSE)
        {
            return std::unexpected(environment::last_system_error());
        }
        return {};
#elifdef __APPLE__
        CFStringRef service_ref{ CFStringCreateWithCString(nullptr, m_app_id.c_str(), kCFStringEncodingUTF8) };
        CFStringRef name_ref{ CFStringCreateWithCString(nullptr, credential.name().c_str(), kCFStringEncodingUTF8) };
        CFStringRef url_ref{ CFStringCreateWithCString(nullptr, credential.url().c_str(), kCFStringEncodingUTF8) };
        CFDataRef username_data{ CFDataCreate(nullptr,
                                              reinterpret_cast<const UInt8*>(credential.username().data()),
                                              static_cast<CFIndex>(credential.username().size())) };
        CFDataRef password_data{ CFDataCreate(nullptr,
                                              reinterpret_cast<const UInt8*>(credential.password().data()),
                                              static_cast<CFIndex>(credential.password().size())) };
        CFMutableDictionaryRef query{
            CFDictionaryCreateMutable(nullptr, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks)
        };
        CFDictionaryAddValue(query, kSecClass, kSecClassGenericPassword);
        CFDictionaryAddValue(query, kSecAttrService, service_ref);
        CFDictionaryAddValue(query, kSecAttrAccount, name_ref);
        CFMutableDictionaryRef attributes{
            CFDictionaryCreateMutable(nullptr, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks)
        };
        CFDictionaryAddValue(attributes, kSecAttrComment, url_ref);
        CFDictionaryAddValue(attributes, kSecAttrGeneric, username_data);
        CFDictionaryAddValue(attributes, kSecValueData, password_data);
        OSStatus status{ SecItemUpdate(query, attributes) };
        CFRelease(attributes);
        CFRelease(query);
        CFRelease(password_data);
        CFRelease(username_data);
        CFRelease(url_ref);
        CFRelease(name_ref);
        CFRelease(service_ref);
        if (status != errSecSuccess)
        {
            CFStringRef message_ref{ SecCopyErrorMessageString(status, nullptr) };
            std::string message{ str(message_ref) };
            if (message_ref != nullptr)
            {
                CFRelease(message_ref);
            }
            return std::unexpected(message);
        }
        return {};
#else
        GError* error{ nullptr };
        char* existing{ secret_password_lookup_sync(&KEYRING_SCHEMA,
                                                    nullptr,
                                                    &error,
                                                    "application",
                                                    m_app_id.c_str(),
                                                    "name",
                                                    credential.name().c_str(),
                                                    nullptr) };
        if (error)
        {
            std::string message{ error->message };
            g_error_free(error);
            return std::unexpected(message);
        }
        if (!existing)
        {
            return std::unexpected<std::string>("The credential could not be found.");
        }
        secret_password_free(existing);
        secret_password_clear_sync(&KEYRING_SCHEMA,
                                   nullptr,
                                   &error,
                                   "application",
                                   m_app_id.c_str(),
                                   "name",
                                   credential.name().c_str(),
                                   nullptr);
        if (error)
        {
            std::string message{ error->message };
            g_error_free(error);
            return std::unexpected(message);
        }
        secret_password_store_sync(&KEYRING_SCHEMA,
                                   SECRET_COLLECTION_DEFAULT,
                                   credential.name().c_str(),
                                   credential.password().c_str(),
                                   nullptr,
                                   &error,
                                   "application",
                                   m_app_id.c_str(),
                                   "name",
                                   credential.name().c_str(),
                                   "url",
                                   credential.url().c_str(),
                                   "username",
                                   credential.username().c_str(),
                                   nullptr);
        if (error)
        {
            std::string message{ error->message };
            g_error_free(error);
            return std::unexpected(message);
        }
        return {};
#endif
    }

    void from_json(const nlohmann::json& json, Credential& credential)
    {
        if (json.contains("name") && json["name"].is_string())
        {
            credential.set_name(json.at("name").get<std::string>());
        }
        else if (json.contains("Name") && json["Name"].is_string())
        {
            credential.set_name(json.at("Name").get<std::string>());
        }
        if (json.contains("url") && json["url"].is_string())
        {
            credential.set_url(json.at("url").get<std::string>());
        }
        else if (json.contains("Url") && json["Url"].is_string())
        {
            credential.set_url(json.at("Url").get<std::string>());
        }
        if (json.contains("username") && json["username"].is_string())
        {
            credential.set_username(json.at("username").get<std::string>());
        }
        else if (json.contains("Username") && json["Username"].is_string())
        {
            credential.set_username(json.at("Username").get<std::string>());
        }
        if (json.contains("password") && json["password"].is_string())
        {
            credential.set_password(json.at("password").get<std::string>());
        }
        else if (json.contains("Password") && json["Password"].is_string())
        {
            credential.set_password(json.at("Password").get<std::string>());
        }
    }

    void to_json(nlohmann::json& json, const Credential& credential)
    {
        json = nlohmann::json{ { "name", credential.name() },
                               { "url", credential.url() },
                               { "username", credential.username() },
                               { "password", credential.password() } };
    }
}
