#pragma once

#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace parabolic::keyring
{
    class Credential
    {
    public:
        Credential() = default;
        Credential(std::string name, std::string url, std::string username, std::string password);
        [[nodiscard]] const std::string& name() const;
        void set_name(const std::string& name);
        [[nodiscard]] const std::string& url() const;
        void set_url(const std::string& url);
        [[nodiscard]] const std::string& username() const;
        void set_username(const std::string& username);
        [[nodiscard]] const std::string& password() const;
        void set_password(const std::string& password);
        friend void from_json(const nlohmann::json& json, Credential& credential);
        friend void to_json(nlohmann::json& json, const Credential& credential);

    private:
        std::string m_name;
        std::string m_url;
        std::string m_username;
        std::string m_password;
    };

    class Store
    {
    public:
        Store(std::string_view app_id);
        [[nodiscard]] bool add(const Credential& credential);
        [[nodiscard]] std::vector<Credential> get_all() const;
        [[nodiscard]] bool remove(const std::string& name);
        [[nodiscard]] bool update(const Credential& credential);

    private:
        mutable std::mutex m_mutex;
        std::string m_app_id;
    };
}
