#include "updater.h"

namespace NickvisionTubeConverter::Models::Update
{
    Updater::Updater(const std::string& linkToConfig, const Version& currentVersion) : m_linkToConfig(linkToConfig), m_currentVersion(currentVersion), m_updateConfig(std::nullopt), m_updateAvailable(false)
    {

    }

    bool Updater::updateAvailable() const
    {
        return m_updateAvailable && m_updateConfig.has_value();
    }

    std::optional<Version> Updater::getLatestVersion() const
    {
        if (m_updateConfig.has_value())
        {
            return m_updateConfig->getLatestVersion();
        }
        return std::nullopt;
    }

    std::string Updater::getChangelog() const
    {
        return m_updateConfig.has_value() ? m_updateConfig->getChangelog() : "";
    }

    bool Updater::checkForUpdates()
    {
        m_updateConfig = UpdateConfig::loadFromUrl(m_linkToConfig);
        if (m_updateConfig.has_value() && m_updateConfig->getLatestVersion() > m_currentVersion)
        {
            m_updateAvailable = true;
        }
        return updateAvailable();
    }
}
