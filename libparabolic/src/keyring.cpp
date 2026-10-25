#include "keyring.h"
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <utility>

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

    void from_json(const nlohmann::json& json, Credential& credential)
    {
        if (json.contains("name") && json["name"].is_string())
        {
            credential.m_name = json.at("name").get<std::string>();
        }
        else if (json.contains("Name") && json["Name"].is_string())
        {
            credential.m_name = json.at("Name").get<std::string>();
        }
        if (json.contains("url") && json["url"].is_string())
        {
            credential.m_url = json.at("url").get<std::string>();
        }
        else if (json.contains("Url") && json["Url"].is_string())
        {
            credential.m_url = json.at("Url").get<std::string>();
        }
        if (json.contains("username") && json["username"].is_string())
        {
            credential.m_username = json.at("username").get<std::string>();
        }
        else if (json.contains("Username") && json["Username"].is_string())
        {
            credential.m_username = json.at("Username").get<std::string>();
        }
        if (json.contains("password") && json["password"].is_string())
        {
            credential.m_password = json.at("password").get<std::string>();
        }
        else if (json.contains("Password") && json["Password"].is_string())
        {
            credential.m_password = json.at("Password").get<std::string>();
        }
    }

    void to_json(nlohmann::json& json, const Credential& credential)
    {
        json = nlohmann::json{
            { "name", credential.m_name },
            { "url", credential.m_url },
            { "username", credential.m_username },
            { "password", credential.m_password },
        };
    }
}
