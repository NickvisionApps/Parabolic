#include "updateconfig.h"
#include <fstream>
#include <unistd.h>
#include <pwd.h>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <json/json.h>

namespace NickvisionTubeConverter::Models::Update
{
    UpdateConfig::UpdateConfig() : m_latestVersion("0.0.0"), m_changelog("")
    {

    }

    std::optional<UpdateConfig> UpdateConfig::loadFromUrl(const std::string& url)
    {
        cURLpp::Cleanup curlCleanup;
        std::ofstream updateConfigFileOut(std::string(getpwuid(getuid())->pw_dir) + "/.config/Nickvision/NickvisionTubeConverter/UpdateConfig.json");
        if (updateConfigFileOut.is_open())
        {
            cURLpp::Easy handle;
            try
            {
                handle.setOpt(cURLpp::Options::Url(url));
                handle.setOpt(cURLpp::Options::WriteStream(&updateConfigFileOut));
                handle.perform();
            }
            catch(...)
            {
                return std::nullopt;
            }
            updateConfigFileOut.close();
        }
        else
        {
            return std::nullopt;
        }
        std::ifstream updateConfigFileIn(std::string(getpwuid(getuid())->pw_dir) + "/.config/Nickvision/NickvisionTubeConverter/UpdateConfig.json");
        if (updateConfigFileIn.is_open())
        {
            UpdateConfig updateConfig;
            Json::Value json;
            try
            {
                updateConfigFileIn >> json;
                updateConfig.setLatestVersion({ json.get("LatestVersion", "0.0.0").asString() });
                updateConfig.setChangelog(json.get("Changelog", "").asString());
            }
            catch (...)
            {
                return std::nullopt;
            }
            return updateConfig;
        }
        return std::nullopt;
    }

    const Version& UpdateConfig::getLatestVersion() const
    {
        return m_latestVersion;
    }

    void UpdateConfig::setLatestVersion(const Version& latestVersion)
    {
        m_latestVersion = latestVersion;
    }

    const std::string& UpdateConfig::getChangelog() const
    {
        return m_changelog;
    }

    void UpdateConfig::setChangelog(const std::string& changelog)
    {
        m_changelog = changelog;
    }
}
