#include "configuration.hpp"
#include <filesystem>
#include <fstream>
#include <adwaita.h>
#include <json/json.h>

using namespace NickvisionTubeConverter::Models;

Configuration::Configuration() : m_configDir{ std::string(g_get_user_config_dir()) + "/Nickvision/NickvisionTubeConverter/" }, m_theme{ Theme::System }, m_previousSaveFolder { "" }, m_previousFileFormat{MediaFileType::MP4}
{
    if(!std::filesystem::exists(m_configDir))
    {
        std::filesystem::create_directories(m_configDir);
    }
    std::ifstream configFile{ m_configDir + "config.json" };
    if(configFile.is_open())
    {
        Json::Value json;
        configFile >> json;
        m_theme = static_cast<Theme>(json.get("Theme", 0).asInt());
        m_previousSaveFolder = json.get("PreviousSaveFolder", "").asString();
        m_previousFileFormat = static_cast<MediaFileType::Value>(json.get("PreviousFileFormat", 0).asInt());
    }
}

Theme Configuration::getTheme() const
{
    return m_theme;
}

void Configuration::setTheme(Theme theme)
{
    m_theme = theme;
}

const std::string& Configuration::getPreviousSaveFolder() const
{
    return m_previousSaveFolder;
}

void Configuration::setPreviousSaveFolder(const std::string& previousSaveFolder)
{
    m_previousSaveFolder = previousSaveFolder;
}

const MediaFileType& Configuration::getPreviousFileFormat() const
{
    return m_previousFileFormat;
}

void Configuration::setPreviousFileForamt(const MediaFileType& previousFileFormat)
{
    m_previousFileFormat = previousFileFormat;
}

void Configuration::save() const
{
    std::ofstream configFile{ m_configDir + "config.json" };
    if(configFile.is_open())
    {
        Json::Value json;
        json["Theme"] = static_cast<int>(m_theme);
        json["PreviousSaveFolder"] = m_previousSaveFolder;
        json["PreviousFileFormat"] = static_cast<int>(m_previousFileFormat);
        configFile << json;
    }
}

