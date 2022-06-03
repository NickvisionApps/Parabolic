#include "configuration.h"
#include <filesystem>
#include <fstream>
#include <unistd.h>
#include <pwd.h>
#include <json/json.h>

using namespace NickvisionTubeConverter::Models;

Configuration::Configuration() : m_configDir{std::string(getpwuid(getuid())->pw_dir) + "/.config/Nickvision/NickvisionTubeConverter/"}, m_theme{Theme::System}, m_isFirstTimeOpen{true}
{
    if (!std::filesystem::exists(m_configDir))
    {
        std::filesystem::create_directories(m_configDir);
    }
    std::ifstream configFile{m_configDir + "config.json"};
    if (configFile.is_open())
    {
        Json::Value json;
        configFile >> json;
        m_theme = static_cast<Theme>(json.get("Theme", 0).asInt());
        m_isFirstTimeOpen = json.get("IsFirstTimeOpen", true).asBool();
    }
}

Theme Configuration::getTheme() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_theme;
}

void Configuration::setTheme(Theme theme)
{
    std::lock_guard<std::mutex> lock{m_mutex};
    m_theme = theme;
}

bool Configuration::getIsFirstTimeOpen() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_isFirstTimeOpen;
}

void Configuration::setIsFirstTimeOpen(bool isFirstTimeOpen)
{
    std::lock_guard<std::mutex> lock{m_mutex};
    m_isFirstTimeOpen = isFirstTimeOpen;
}

void Configuration::save() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    std::ofstream configFile{m_configDir + "config.json"};
    if (configFile.is_open())
    {
        Json::Value json;
        json["Theme"] = static_cast<int>(m_theme);
        json["IsFirstTimeOpen"] = m_isFirstTimeOpen;
        configFile << json;
    }
}
