#include "configuration.h"
#include <filesystem>
#include <fstream>
#include <optional>
#include <unistd.h>
#include <pwd.h>
#include <json/json.h>

using namespace NickvisionTubeConverter::Models;

Configuration::Configuration() : m_configDir{std::string(getpwuid(getuid())->pw_dir) + "/.config/Nickvision/NickvisionTubeConverter/"}, m_theme{Theme::System}, m_maxNumberOfActiveDownloads{5}, m_previousSaveFolder{}, m_previousFileFormat{MediaFileType::MP4}
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
        m_maxNumberOfActiveDownloads = json.get("MaxNumberOfActiveDownloads", 5).asUInt();
        m_previousSaveFolder = json.get("PreviousSaveFolder", "").asString();
        std::optional<MediaFileType> mediaFileType = MediaFileType::parse(json.get("PreviousFileFormat", "mp4").asString());
        m_previousFileFormat = mediaFileType.has_value() ? mediaFileType.value() : MediaFileType(MediaFileType::MP4);
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

unsigned int Configuration::getMaxNumberOfActiveDownloads() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_maxNumberOfActiveDownloads;
}

void Configuration::setMaxNumberOfActiveDownloads(unsigned int maxNumberOfActiveDownloads)
{
    std::lock_guard<std::mutex> lock{m_mutex};
    m_maxNumberOfActiveDownloads = maxNumberOfActiveDownloads;
}

const std::string& Configuration::getPreviousSaveFolder() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_previousSaveFolder;
}

void Configuration::setPreviousSaveFolder(const std::string& previousSaveFolder)
{
    std::lock_guard<std::mutex> lock{m_mutex};
    m_previousSaveFolder = previousSaveFolder;
}

const MediaFileType& Configuration::getPreviousFileFormat() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_previousFileFormat;
}

void Configuration::setPreviousFileFormat(const MediaFileType& previousFileFormat)
{
    std::lock_guard<std::mutex> lock{m_mutex};
    m_previousFileFormat = previousFileFormat;
}

void Configuration::save() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    std::ofstream configFile{m_configDir + "config.json"};
    if (configFile.is_open())
    {
        Json::Value json;
        json["Theme"] = static_cast<int>(m_theme);
        json["MaxNumberOfActiveDownloads"] = m_maxNumberOfActiveDownloads;
        json["PreviousSaveFolder"] = m_previousSaveFolder;
        json["PreviousFileFormat"] = m_previousFileFormat.toString();
        configFile << json;
    }
}
