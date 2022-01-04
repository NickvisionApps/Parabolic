#include "configuration.h"
#include <filesystem>
#include <fstream>
#include <unistd.h>
#include <pwd.h>
#include <json/json.h>

namespace NickvisionTubeConverter::Models
{
    Configuration::Configuration() : m_configDir(std::string(getpwuid(getuid())->pw_dir) + "/.config/Nickvision/NickvisionTubeConverter/"), m_maxNumberOfActiveDownloads(5), m_previousSaveFolder(""), m_previousFileFormat(0)
    {
        if (!std::filesystem::exists(m_configDir))
        {
            std::filesystem::create_directories(m_configDir);
        }
        std::ifstream configFile(m_configDir + "config.json");
        if (configFile.is_open())
        {
            Json::Value json;
            try
            {
                configFile >> json;
                setMaxNumberOfActiveDownloads(json.get("MaxNumberOfActiveDownloads", 5).asInt());
                setPreviousSaveFolder(json.get("PreviousSaveFolder", "").asString());
                setPreviousFileFormat(json.get("PreviousFileFormat", 0).asInt());
            }
            catch (...) { }
        }
    }

    int Configuration::getMaxNumberOfActiveDownloads() const
    {
        return m_maxNumberOfActiveDownloads;
    }

    void Configuration::setMaxNumberOfActiveDownloads(int maxNumberOfActiveDownloads)
    {
        m_maxNumberOfActiveDownloads = maxNumberOfActiveDownloads;
    }

    const std::string& Configuration::getPreviousSaveFolder() const
    {
        return m_previousSaveFolder;
    }

    void Configuration::setPreviousSaveFolder(const std::string& previousSaveFolder)
    {
        m_previousSaveFolder = previousSaveFolder;
    }

    int Configuration::getPreviousFileFormat() const
    {
        return m_previousFileFormat;
    }

    void Configuration::setPreviousFileFormat(int previousFileFormat)
    {
        m_previousFileFormat = previousFileFormat;
    }

    void Configuration::save() const
    {
        std::ofstream configFile(m_configDir + "config.json");
        if (configFile.is_open())
        {
            Json::Value json;
            json["MaxNumberOfActiveDownloads"] = getMaxNumberOfActiveDownloads();
            json["PreviousSaveFolder"] = getPreviousSaveFolder();
            json["PreviousFileFormat"] = getPreviousFileFormat();
            configFile << json;
        }
    }
}
