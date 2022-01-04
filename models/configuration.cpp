#include "configuration.h"
#include <filesystem>
#include <fstream>
#include <unistd.h>
#include <pwd.h>
#include <json/json.h>

namespace NickvisionTubeConverter::Models
{
    Configuration::Configuration() : m_configDir(std::string(getpwuid(getuid())->pw_dir) + "/.config/Nickvision/NickvisionTubeConverter/"), m_isFirstTimeOpen(true)
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
                setIsFirstTimeOpen(json.get("IsFirstTimeOpen", true).asBool());
            }
            catch (...) { }
        }
    }

    bool Configuration::isFirstTimeOpen() const
    {
        return m_isFirstTimeOpen;
    }

    void Configuration::setIsFirstTimeOpen(bool isFirstTimeOpen)
    {
        m_isFirstTimeOpen = isFirstTimeOpen;
    }

    void Configuration::save() const
    {
        std::ofstream configFile(m_configDir + "config.json");
        if (configFile.is_open())
        {
            Json::Value json;
            json["IsFirstTimeOpen"] = isFirstTimeOpen();
            configFile << json;
        }
    }
}
