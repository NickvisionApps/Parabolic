#include "models/ytdlpmanager.h"
#include <thread>
#include <libnick/filesystem/userdirectories.h>
#include <libnick/localization/gettext.h>
#include <libnick/notifications/appnotification.h>
#include <libnick/system/environment.h>
#ifndef _WIN32
#include <sys/stat.h>
#endif

#define BUNDLED_YTDLP_VERSION Version(2025, 11, 12)

#ifdef _WIN32
#ifdef _M_ARM64
#define UPDATED_YTDLP_EXECUTABLE_NAME "yt-dlp_arm64.exe"
#else
#define UPDATED_YTDLP_EXECUTABLE_NAME "yt-dlp.exe"
#endif
#elif defined(__linux__)
#ifdef __aarch64__
#define UPDATED_YTDLP_EXECUTABLE_NAME "yt-dlp_linux_aarch64"
#else
#define UPDATED_YTDLP_EXECUTABLE_NAME "yt-dlp_linux"
#endif
#elif defined(__APPLE__)
#define UPDATED_YTDLP_EXECUTABLE_NAME "yt-dlp_macos"
#endif

using namespace Nickvision::Events;
using namespace Nickvision::Filesystem;
using namespace Nickvision::Notifications;
using namespace Nickvision::System;
using namespace Nickvision::Update;

namespace Nickvision::TubeConverter::Shared::Models
{
    YtdlpManager::YtdlpManager(Configuration& config)
        : m_config{ config },
        m_updater{ "https://github.com/yt-dlp/yt-dlp/" },
#ifndef __linux__
        m_bundledYtdlpVersion{ BUNDLED_YTDLP_VERSION }
#else
        m_bundledYtdlpVersion{ Environment::getDeploymentMode() == DeploymentMode::Local ? Version(0, 0, 0) : BUNDLED_YTDLP_VERSION }
#endif
    {

    }

    Event<ParamEventArgs<Version>>& YtdlpManager::updateAvailable()
    {
        return m_updateAvailable;
    }

    Event<ParamEventArgs<double>>& YtdlpManager::updateProgressChanged()
    {
        return m_updateProgressChanged;
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
        getExecutablePath(); // Validate the executable path from config
        std::thread worker{ [this]()
        {
            m_latestYtdlpVersion = m_updater.fetchCurrentVersion(VersionType::Stable);
            if(!m_latestYtdlpVersion.empty())
            {
                if(m_latestYtdlpVersion > m_bundledYtdlpVersion && m_latestYtdlpVersion > m_config.getInstalledYtdlpVersion())
                {
                    m_updateAvailable.invoke({ m_latestYtdlpVersion });
                }
            }
        } };
        worker.detach();
    }

    void YtdlpManager::startUpdateDownload()
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
            cpr::ProgressCallback progressCallback{ [this](curl_off_t downloadTotal, curl_off_t downloadNow, curl_off_t, curl_off_t, intptr_t) -> bool
            {
                if(downloadTotal == 0)
                {
                    return true;
                }
                double progress{ static_cast<double>(static_cast<long double>(downloadNow) / static_cast<long double>(downloadTotal)) };
                if(progress != 1.0)
                {
                    m_updateProgressChanged.invoke({ progress });
                }
                return true;
            } };
            m_updateProgressChanged.invoke({ 0.0 });
            bool res{ m_updater.downloadUpdate(VersionType::Stable, ytdlpPath, UPDATED_YTDLP_EXECUTABLE_NAME, true, progressCallback) };
            m_updateProgressChanged.invoke({ 1.0 });
            if(res)
            {
#ifndef _WIN32
                chmod(ytdlpPath.string().c_str(), 0777);
#endif
                m_config.setInstalledYtdlpVersion(m_latestYtdlpVersion);
                m_config.save();
            }
            else
            {
                AppNotification::send({ _("Unable to download yt-dlp update"), NotificationSeverity::Error });
            }
        } };
        worker.detach();
    }
}
