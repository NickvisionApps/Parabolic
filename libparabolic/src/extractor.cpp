#include "extractor.h"
#include <algorithm>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include "cookies.h"

namespace parabolic::extractor
{
    cookies::Browser Options::cookies_browser() const
    {
        return m_cookies_browser;
    }

    void Options::set_cookies_browser(cookies::Browser value)
    {
        m_cookies_browser = value;
    }

    const std::string& Options::cookies_path() const
    {
        return m_cookies_path;
    }

    void Options::set_cookies_path(const std::string& value)
    {
        m_cookies_path = value;
    }

    const std::string& Options::proxy_url() const
    {
        return m_proxy_url;
    }

    void Options::set_proxy_url(const std::string& value)
    {
        m_proxy_url = value;
    }

    std::optional<int> Options::speed_limit() const
    {
        return m_speed_limit;
    }

    void Options::set_speed_limit(std::optional<int> value)
    {
        m_speed_limit = value;
    }

    bool Options::translate_metadata_and_chapters() const
    {
        return m_translate_metadata_and_chapters;
    }

    void Options::set_translate_metadata_and_chapters(bool value)
    {
        m_translate_metadata_and_chapters = value;
    }

    bool Options::use_part_files() const
    {
        return m_use_part_files;
    }

    void Options::set_use_part_files(bool value)
    {
        m_use_part_files = value;
    }

    bool Options::youtube_sponsor_block() const
    {
        return m_youtube_sponsor_block;
    }

    void Options::set_youtube_sponsor_block(bool value)
    {
        m_youtube_sponsor_block = value;
    }

    void from_json(const nlohmann::json& json, Options& options)
    {
        if (json.contains("cookiesBrowser") && json["cookiesBrowser"].is_number_integer())
        {
            int value{ json.at("cookiesBrowser").get<int>() };
            if (value >= static_cast<int>(cookies::Browser::None) && value <= static_cast<int>(cookies::Browser::Whale))
            {
                options.m_cookies_browser = static_cast<cookies::Browser>(value);
            }
        }
        else if (json.contains("CookiesBrowser") && json["CookiesBrowser"].is_number_integer())
        {
            int value{ json.at("CookiesBrowser").get<int>() };
            if (value >= static_cast<int>(cookies::Browser::None) && value <= static_cast<int>(cookies::Browser::Whale))
            {
                options.m_cookies_browser = static_cast<cookies::Browser>(value);
            }
        }
        if (json.contains("cookiesPath") && json["cookiesPath"].is_string())
        {
            options.m_cookies_path = json.at("cookiesPath").get<std::string>();
        }
        else if (json.contains("CookiesPath") && json["CookiesPath"].is_string())
        {
            options.m_cookies_path = json.at("CookiesPath").get<std::string>();
        }
        if (json.contains("proxyUrl") && json["proxyUrl"].is_string())
        {
            options.m_proxy_url = json.at("proxyUrl").get<std::string>();
        }
        else if (json.contains("ProxyUrl") && json["ProxyUrl"].is_string())
        {
            options.m_proxy_url = json.at("ProxyUrl").get<std::string>();
        }
        if (json.contains("speedLimit"))
        {
            if (json["speedLimit"].is_number_integer())
            {
                options.m_speed_limit = std::clamp(json["speedLimit"].get<int>(), 512, 10240);
            }
            else if (json["speedLimit"].is_null())
            {
                options.m_speed_limit = std::nullopt;
            }
        }
        if (json.contains("translateMetadataAndChapters") && json["translateMetadataAndChapters"].is_boolean())
        {
            options.m_translate_metadata_and_chapters = json.at("translateMetadataAndChapters").get<bool>();
        }
        else if (json.contains("TranslateMetadataAndChapters") && json["TranslateMetadataAndChapters"].is_boolean())
        {
            options.m_translate_metadata_and_chapters = json.at("TranslateMetadataAndChapters").get<bool>();
        }
        if (json.contains("usePartFiles") && json["usePartFiles"].is_boolean())
        {
            options.m_use_part_files = json.at("usePartFiles").get<bool>();
        }
        else if (json.contains("UsePartFiles") && json["UsePartFiles"].is_boolean())
        {
            options.m_use_part_files = json.at("UsePartFiles").get<bool>();
        }
        if (json.contains("youTubeSponsorBlock") && json["youTubeSponsorBlock"].is_boolean())
        {
            options.m_youtube_sponsor_block = json.at("youTubeSponsorBlock").get<bool>();
        }
        else if (json.contains("YouTubeSponsorBlock") && json["YouTubeSponsorBlock"].is_boolean())
        {
            options.m_youtube_sponsor_block = json.at("YouTubeSponsorBlock").get<bool>();
        }
    }

    void to_json(nlohmann::json& json, const Options& options)
    {
        json = nlohmann::json{
            { "cookiesBrowser", static_cast<int>(options.m_cookies_browser) },
            { "cookiesPath", options.m_cookies_path },
            { "proxyUrl", options.m_proxy_url },
            {
                    "speedLimit",
                    options.m_speed_limit.has_value() ? nlohmann::json(options.m_speed_limit.value())
                                                      : nlohmann::json(nullptr),
            },
            { "translateMetadataAndChapters", options.m_translate_metadata_and_chapters },
            { "usePartFiles", options.m_use_part_files },
            { "youTubeSponsorBlock", options.m_youtube_sponsor_block },
        };
    }
}
