#pragma once

#include <filesystem>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include "media.h"
#include "subtitle.h"

namespace parabolic::configuration
{
    enum class Theme : std::uint8_t
    {
        Light,
        Dark,
        System
    };

    class Configuration
    {
    public:
        Configuration(const std::string& app_name);
        [[nodiscard]] Theme theme() const;
        void set_theme(Theme value);
        [[nodiscard]] bool automatically_check_for_updates() const;
        void set_automatically_check_for_updates(bool value);
        [[nodiscard]] bool allow_preview_updates() const;
        void set_allow_preview_updates(bool value);
        [[nodiscard]] std::string translation_language() const;
        void set_translation_language(const std::string& value);
        [[nodiscard]] std::string str() const;

    private:
        void save() const;
        mutable std::mutex m_mutex;
        std::filesystem::path m_path;
        Theme m_theme;
        bool m_automatically_check_for_updates;
        bool m_allow_preview_updates;
        std::string m_translation_language;
        friend void from_json(const nlohmann::json& json, Configuration& config);
        friend void to_json(nlohmann::json& json, const Configuration& config);
    };

    inline void from_json(const nlohmann::json& json, Configuration& config)
    {
        if (json.contains("theme") && json["theme"].is_number_integer())
        {
            config.m_theme = static_cast<Theme>(json["theme"].get<int>());
        }
        else if (json.contains("Theme") && json["Theme"].is_number_integer())
        {
            config.m_theme = static_cast<Theme>(json["Theme"].get<int>());
        }
        if (json.contains("automaticallyCheckForUpdates") && json["automaticallyCheckForUpdates"].is_boolean())
        {
            config.m_automatically_check_for_updates = json["automaticallyCheckForUpdates"].get<bool>();
        }
        else if (json.contains("AutomaticallyCheckForUpdates") && json["AutomaticallyCheckForUpdates"].is_boolean())
        {
            config.m_automatically_check_for_updates = json["AutomaticallyCheckForUpdates"].get<bool>();
        }
        if (json.contains("allowPreviewUpdates") && json["allowPreviewUpdates"].is_boolean())
        {
            config.m_allow_preview_updates = json["allowPreviewUpdates"].get<bool>();
        }
        else if (json.contains("AllowPreviewUpdates") && json["AllowPreviewUpdates"].is_boolean())
        {
            config.m_allow_preview_updates = json["AllowPreviewUpdates"].get<bool>();
        }
        if (json.contains("translationLanguage") && json["translationLanguage"].is_string())
        {
            config.m_translation_language = json["translationLanguage"].get<std::string>();
        }
        else if (json.contains("TranslationLanguage") && json["TranslationLanguage"].is_string())
        {
            config.m_translation_language = json["TranslationLanguage"].get<std::string>();
        }
    }

    inline void to_json(nlohmann::json& json, const Configuration& config)
    {
        json = nlohmann::json{ { "theme", static_cast<int>(config.m_theme) },
                               { "automaticallyCheckForUpdates", config.m_automatically_check_for_updates },
                               { "allowPreviewUpdates", config.m_allow_preview_updates },
                               { "translationLanguage", config.m_translation_language } };
    }
}
