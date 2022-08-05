#include "UpdateConfig.h"
#include <fstream>
#include <QStandardPaths>
#include <json/json.h>
#include "../Helpers/CurlHelpers.h"

using namespace NickvisionTubeConverter::Helpers;

namespace NickvisionTubeConverter::Update
{
	UpdateConfig::UpdateConfig() : m_latestVersion{ "0.0.0" }, m_changelog{ "" }, m_linkToSetupExe{ "" }
	{

	}

    std::optional<UpdateConfig> UpdateConfig::loadFromUrl(const std::string& url)
    {
        std::string configFilePath{ QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString() + "/updateConfig.json" };
        if (!CurlHelpers::downloadFile(url, configFilePath))
        {
            return std::nullopt;
        }
        std::ifstream updateConfigFileIn{ configFilePath };
        if (updateConfigFileIn.is_open())
        {
            Json::Value json;
            updateConfigFileIn >> json;
            try
            {
                UpdateConfig updateConfig;
                updateConfig.m_latestVersion = { json.get("LatestVersion", "0.0.0").asString() };
                updateConfig.m_changelog = json.get("Changelog", "").asString();
                updateConfig.m_linkToSetupExe = json.get("LinkToSetupExe", "").asString();
                return updateConfig;
            }
            catch (...) 
            { 
                return std::nullopt;
            }
        }
        else
        {
            return std::nullopt;
        }
    }

    const Version& UpdateConfig::getLatestVersion() const
    {
        return m_latestVersion;
    }

    const std::string& UpdateConfig::getChangelog() const
    {
        return m_changelog;
    }

    const std::string& UpdateConfig::getLinkToSetupExe() const
    {
        return m_linkToSetupExe;
    }
}