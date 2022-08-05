#include "Updater.h"
#include <filesystem>
#include <fstream>
#include <QStandardPaths>
#include <QProcess>
#include "../Helpers/CurlHelpers.h"

using namespace NickvisionTubeConverter::Helpers;

namespace NickvisionTubeConverter::Update
{
	Updater::Updater(const std::string& linkToConfig, const Version& currentVersion) : m_linkToConfig{ linkToConfig }, m_currentVersion{ currentVersion }, m_updateConfig{ std::nullopt }, m_updateAvailable{ false }, m_updateSuccessful{ false }
	{

	}

    bool Updater::getUpdateAvailable() const
    {
        return m_updateAvailable;
    }

    Version Updater::getLatestVersion() const
    {
        return m_updateConfig.has_value() ? m_updateConfig->getLatestVersion() : Version("-1.-1.-1" );
    }

    std::string Updater::getChangelog() const
    {
        return m_updateConfig.has_value() ? m_updateConfig->getChangelog() : "";
    }

    bool Updater::getUpdateSuccessful() const
    {
        return m_updateSuccessful;
    }

    bool Updater::checkForUpdates()
    {
        m_updateConfig = UpdateConfig::loadFromUrl(m_linkToConfig);
        m_updateAvailable = m_updateConfig.has_value() && m_updateConfig->getLatestVersion() > m_currentVersion;
        return m_updateAvailable;
    }

    bool Updater::windowsUpdate(QMainWindow* window)
    {
        if (!m_updateAvailable)
        {
            m_updateSuccessful = false;
            return m_updateSuccessful;
        }
        std::string downloadsDir{ QStandardPaths::writableLocation(QStandardPaths::DownloadLocation).toStdString() };
        if (std::filesystem::exists(downloadsDir))
        {
            std::filesystem::create_directories(downloadsDir);
        }
        std::string exePath{ downloadsDir + "/Setup.exe" };
        m_updateSuccessful = CurlHelpers::downloadFile(m_updateConfig->getLinkToSetupExe(), exePath);
        QProcess::execute(QString::fromStdString(exePath));
        window->close();
        return m_updateSuccessful;
        m_updateSuccessful = false;
        return m_updateSuccessful;
    }
}