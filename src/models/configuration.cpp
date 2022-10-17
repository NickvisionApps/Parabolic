#include "configuration.hpp"
#include <filesystem>
#include <fstream>
#include <adwaita.h>
#include <json/json.h>

using namespace NickvisionTubeConverter::Models;

Configuration::Configuration() : m_configDir{ std::string(g_get_user_config_dir()) + "/Nickvision/NickvisionTubeConverter/" }, m_theme{ Theme::System }, m_previousSaveFolder { "" }, m_previousFileType{ MediaFileType::MP4 }, m_embedMetadata{ true }
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
        m_previousFileType = static_cast<MediaFileType::Value>(json.get("PreviousFileType", 0).asInt());
        m_embedMetadata = json.get("EmbedMetadata", true).asBool();
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

const MediaFileType& Configuration::getPreviousFileType() const
{
    return m_previousFileType;
}

void Configuration::setPreviousFileType(const MediaFileType& previousFileType)
{
    m_previousFileType = previousFileType;
}

bool Configuration::getEmbedMetadata() const
{
    return m_embedMetadata;
}

void Configuration::setEmbedMetadata(bool embedMetadata)
{
    m_embedMetadata = embedMetadata;
}

void Configuration::save() const
{
    std::ofstream configFile{ m_configDir + "config.json" };
    if(configFile.is_open())
    {
        Json::Value json;
        json["Theme"] = static_cast<int>(m_theme);
        json["PreviousSaveFolder"] = m_previousSaveFolder;
        json["PreviousFileType"] = static_cast<int>(m_previousFileType);
        json["EmbedMetadata"] = m_embedMetadata;
        configFile << json;
    }
}

