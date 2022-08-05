#include "Configuration.h"
#include <filesystem>
#include <fstream>
#include <QSettings>
#include <QStandardPaths>
#include <json/json.h>

namespace NickvisionTubeConverter::Models
{
    Configuration::Configuration() : m_configDir{ QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString() }, m_theme{ Theme::System }, m_alwaysStartOnHomePage{ true }
    {
        if (!std::filesystem::exists(m_configDir))
        {
            std::filesystem::create_directories(m_configDir);
        }
        std::ifstream configFile{ m_configDir + "/config.json" };
        if (configFile.is_open())
        {
            Json::Value json;
            configFile >> json;
            try
            {
                m_theme = static_cast<Theme>(json.get("Theme", 2).asInt());
                m_alwaysStartOnHomePage = json.get("AlwaysStartOnHomePage", true).asBool();
            }
            catch (...) { }
        }
    }

    Configuration& Configuration::getInstance()
    {
        static Configuration instance;
        return instance;
    }

    Theme Configuration::getTheme(bool calculateSystemTheme) const
    {
        if (!calculateSystemTheme || m_theme != Theme::System)
        {
            return m_theme;
        }
        QSettings regKeyTheme{ "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat };
        return regKeyTheme.value("AppsUseLightTheme").toBool() ? Theme::Light : Theme::Dark;
    }

    void Configuration::setTheme(Theme theme)
    {
        m_theme = theme;
    }

    bool Configuration::getAlwaysStartOnHomePage() const
    {
        return m_alwaysStartOnHomePage;
    }

    void Configuration::setAlwaysStartOnHomePage(bool alwaysStartOnHomePage)
    {
        m_alwaysStartOnHomePage = alwaysStartOnHomePage;
    }

    void Configuration::save() const
    {
        std::ofstream configFile{ m_configDir + "/config.json" };
        if (configFile.is_open())
        {
            Json::Value json;
            json["Theme"] = static_cast<int>(m_theme);
            json["AlwaysStartOnHomePage"] = m_alwaysStartOnHomePage;
            configFile << json;
        }
    }
}