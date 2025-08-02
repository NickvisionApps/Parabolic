#include "models/ytdlpmanager.h"
#include <thread>
#include <libnick/filesystem/userdirectories.h>
#include <libnick/localization/gettext.h>
#include <libnick/notifications/appnotification.h>
#include <libnick/system/environment.h>

using namespace Nickvision::Filesystem;
using namespace Nickvision::Notifications;
using namespace Nickvision::System;
using namespace Nickvision::Update;

namespace Nickvision::TubeConverter::Shared::Models
{
    YtdlpManager::YtdlpManager(Configuration& config)
        : m_config{ config },
        m_updater{ "https://github.com/yt-dlp/yt-dlp/" },
        m_bundledYtdlpVersion{ 2025, 7, 21 }
    {

    }

    const std::filesystem::path& YtdlpManager::getExecutablePath() const
    {
#ifdef PORTABLE_BUILD
        return Environment::findDependency("yt-dlp", DependencySearchOption::App);
#else
        if(m_config.getInstalledYtdlpVersion() > m_bundledYtdlpVersion)
        {
            const std::filesystem::path& local{ Environment::findDependency("yt-dlp", DependencySearchOption::Local) };
            if(std::filesystem::exists(local))
            {
                return local;
            }
            else
            {
                m_config.setInstalledYtdlpVersion({});
            }
        }
        return Environment::findDependency("yt-dlp", DependencySearchOption::Global);
#endif
    }

    void YtdlpManager::checkForUpdates()
    {
        std::thread worker{ [this]()
        {
            m_latestYtdlpVersion = m_updater.fetchCurrentVersion(VersionType::Stable);
            if(!m_latestYtdlpVersion.empty())
            {
                if(m_latestYtdlpVersion > m_bundledYtdlpVersion && m_latestYtdlpVersion > m_config.getInstalledYtdlpVersion())
                {
                    AppNotification::send({ _("New version of yt-dlp available"), NotificationSeverity::Success, "ytdlp" });
                }
            }
        } };
        worker.detach();
    }

    void YtdlpManager::downloadUpdate()
    {
        std::thread worker{ [this]()
        {
#ifdef _WIN32
#ifdef PORTABLE_BUILD
            std::filesystem::path ytdlpPath{ Environment::getExecutableDirectory() / "yt-dlp.exe" };
#else
            std::filesystem::path ytdlpPath{ UserDirectories::get(UserDirectory::LocalData) / "yt-dlp.exe" };
#endif
#else
#ifdef PORTABLE_BUILD
            std::filesystem::path ytdlpPath{ Environment::getExecutableDirectory() / "yt-dlp" };
#else
            std::filesystem::path ytdlpPath{ UserDirectories::get(UserDirectory::LocalData) / "yt-dlp" };
#endif
#endif
#ifdef _WIN32
            if(m_updater.downloadUpdate(VersionType::Stable, ytdlpPath, "yt-dlp.exe", true))
#elif defined(__APPLE__)
            if(m_updater.downloadUpdate(VersionType::Stable, ytdlpPath, "yt-dlp_macos", true))
#else
            if(m_updater.downloadUpdate(VersionType::Stable, ytdlpPath, "yt-dlp", true))
#endif
            {
                m_config.setInstalledYtdlpVersion(m_latestYtdlpVersion);
                m_config.save();
                AppNotification::send({ _("yt-dlp updated successfully"), NotificationSeverity::Success });
            }
            else
            {
                AppNotification::send({ _("Unable to download yt-dlp update"), NotificationSeverity::Error });
            }
        } };
        worker.detach();
    }
}