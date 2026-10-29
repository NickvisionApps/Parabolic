#include "translation.h"
#include <algorithm>
#include <clocale>
#include <filesystem>
#include <libintl.h>
#include <mutex>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>
#include "environment.h"
#include "string_manip.h"

#define FOLLOW_SYSTEM_LANGUAGE "C"

namespace
{
    std::string normalize_language(std::string language)
    {
        language = language.substr(0, language.find('.'));
        language = language.substr(0, language.find('@'));
        std::ranges::replace(language, '-', '_');
        if (language.empty() || language == FOLLOW_SYSTEM_LANGUAGE || language == "POSIX")
        {
            return "en_US";
        }
        return language;
    }
}

namespace parabolic::translation
{
    Options::Options()
        : m_language{ FOLLOW_SYSTEM_LANGUAGE }
    {
    }

    const std::string& Options::language() const
    {
        return m_language;
    }

    void Options::set_language(const std::string& value)
    {
        m_language = !value.empty() ? value : FOLLOW_SYSTEM_LANGUAGE;
    }

    Translator::Translator(const std::string& app_short_name, std::filesystem::path locale_dir)
        : m_domain_name{ string_manip::lower(app_short_name) },
          m_locale_dir{ !locale_dir.empty() ? std::move(locale_dir) : environment::executable_directory() / "locale" },
          m_available_languages{ "en_US" },
          m_language{ "en_US" }
    {
        if (std::filesystem::is_directory(m_locale_dir))
        {
            for (const std::filesystem::directory_entry& entry: std::filesystem::directory_iterator(m_locale_dir))
            {
                if (entry.is_directory() &&
                    std::filesystem::exists(entry.path() / "LC_MESSAGES" / (m_domain_name + ".mo")))
                {
                    std::string name{ entry.path().filename().string() };
                    if (std::ranges::find(m_available_languages, name) == m_available_languages.end())
                    {
                        m_available_languages.push_back(std::move(name));
                    }
                }
            }
        }
        std::ranges::sort(m_available_languages);
        std::setlocale(LC_ALL, "");
#ifdef _WIN32
        wbindtextdomain(m_domain_name.c_str(), m_locale_dir.c_str());
#else
        bindtextdomain(m_domain_name.c_str(), m_locale_dir.string().c_str());
#endif
        bind_textdomain_codeset(m_domain_name.c_str(), "UTF-8");
        textdomain(m_domain_name.c_str());
    }

    const std::string& Translator::follow_system_language()
    {
        static std::string follow{ FOLLOW_SYSTEM_LANGUAGE };
        return follow;
    }

    const std::vector<std::string>& Translator::available_languages() const
    {
        return m_available_languages;
    }

    std::string Translator::language() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_language;
    }

    const Options& Translator::options() const
    {
        return m_options;
    }

    void Translator::set_options(const Options& options)
    {
        const std::string& language{ options.language() };
        std::string resolved;
        if (language == FOLLOW_SYSTEM_LANGUAGE)
        {
            resolved = normalize_language(environment::locale());
            for (const char* name: { "LC_ALL", "LC_MESSAGES", "LANG" })
            {
                std::string env{ environment::variable(name) };
                if (!env.empty())
                {
                    resolved = normalize_language(env);
                    break;
                }
            }
        }
        else
        {
            resolved = normalize_language(language);
        }
        if (resolved != "en_US" && std::ranges::find(m_available_languages, resolved) == m_available_languages.end())
        {
            std::string base{ resolved.substr(0, resolved.find('_')) };
            if (base != resolved && std::ranges::find(m_available_languages, base) != m_available_languages.end())
            {
                resolved = base;
            }
            else
            {
                resolved = "en_US";
            }
        }
        std::scoped_lock lock{ m_mutex };
        environment::set_variable("LANGUAGE", resolved);
        m_options = options;
        m_language = std::move(resolved);
    }

    const char* Translator::_(const char* msgid) const
    {
        std::scoped_lock lock{ m_mutex };
        if (m_language == "en_US")
        {
            return msgid;
        }
        return ::dgettext(m_domain_name.c_str(), msgid);
    }

    const char* Translator::_n(const char* msgid, const char* msgid_plural, unsigned long n) const
    {
        std::scoped_lock lock{ m_mutex };
        if (m_language == "en_US")
        {
            return n == 1 ? msgid : msgid_plural;
        }
        return ::dngettext(m_domain_name.c_str(), msgid, msgid_plural, n);
    }

    const char* Translator::_p(const char* context, const char* msgid) const
    {
        std::scoped_lock lock{ m_mutex };
        if (m_language == "en_US")
        {
            return msgid;
        }
        std::string ctx{ std::string{ context } + "\004" + msgid };
        const char* result{ ::dcgettext(m_domain_name.c_str(), ctx.c_str(), LC_MESSAGES) };
        return result == ctx.c_str() ? msgid : result;
    }

    const char* Translator::_pn(const char* context, const char* msgid, const char* msgid_plural, unsigned long n) const
    {
        std::scoped_lock lock{ m_mutex };
        if (m_language == "en_US")
        {
            return n == 1 ? msgid : msgid_plural;
        }
        std::string ctx{ std::string{ context } + "\004" + msgid };
        const char* result{ ::dcngettext(m_domain_name.c_str(), ctx.c_str(), msgid_plural, n, LC_MESSAGES) };
        if (result == ctx.c_str() || result == msgid_plural)
        {
            return n == 1 ? msgid : msgid_plural;
        }
        return result;
    }

    bool is_youtube_supported_language(const std::string& language)
    {
        static std::unordered_set<std::string> supported{
            "af",    "am", "ar",    "as",    "az",    "be", "bg",     "bn",    "bs",      "ca",    "cs", "da",
            "de",    "el", "en",    "en-GB", "en-IN", "es", "es-419", "es-US", "et",      "eu",    "fa", "fi",
            "fil",   "fr", "fr-CA", "gl",    "gu",    "hi", "hr",     "hu",    "hy",      "id",    "is", "it",
            "iw",    "ja", "ka",    "kk",    "km",    "kn", "ko",     "ky",    "lo",      "lt",    "lv", "mk",
            "ml",    "mn", "mr",    "ms",    "my",    "ne", "nl",     "no",    "or",      "pa",    "pl", "pt",
            "pt-PT", "ro", "ru",    "si",    "sk",    "sl", "sq",     "sr",    "sr-Latn", "sv",    "sw", "ta",
            "te",    "th", "tr",    "uk",    "ur",    "uz", "vi",     "zh-CN", "zh-HK",   "zh-TW", "zu",
        };
        return supported.contains(language);
    }

    void from_json(const nlohmann::json& json, Options& options)
    {
        if (json.contains("translationLanguage") && json["translationLanguage"].is_string())
        {
            options.set_language(json.at("translationLanguage").get<std::string>());
        }
        else if (json.contains("TranslationLanguage") && json["TranslationLanguage"].is_string())
        {
            options.set_language(json.at("TranslationLanguage").get<std::string>());
        }
    }

    void to_json(nlohmann::json& json, const Options& options)
    {
        json = nlohmann::json{
            { "translationLanguage", options.m_language },
        };
    }
}
