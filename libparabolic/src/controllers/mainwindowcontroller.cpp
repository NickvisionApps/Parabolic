﻿#include "controllers/mainwindowcontroller.h"
#include <sstream>
#include <thread>
#include <libnick/filesystem/userdirectories.h>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/documentation.h>
#include <libnick/localization/gettext.h>
#include <libnick/notifications/appnotification.h>
#include <libnick/notifications/shellnotification.h>
#include <libnick/system/environment.h>
#include "models/configuration.h"
#include "models/downloadhistory.h"
#include "models/downloadrecoveryqueue.h"
#ifdef _WIN32
#include <windows.h>
#endif

#define CONFIG_FILE_KEY "config"

using namespace Nickvision::App;
using namespace Nickvision::Events;
using namespace Nickvision::Filesystem;
using namespace Nickvision::Helpers;
using namespace Nickvision::Keyring;
using namespace Nickvision::Localization;
using namespace Nickvision::Notifications;
using namespace Nickvision::System;
using namespace Nickvision::TubeConverter::Shared::Events;
using namespace Nickvision::TubeConverter::Shared::Models;
using namespace Nickvision::Update;

namespace Nickvision::TubeConverter::Shared::Controllers
{
    MainWindowController::MainWindowController(const std::vector<std::string>& args)
        : m_started{ false },
        m_args{ args },
        m_appInfo{ "org.nickvision.tubeconverter", "Nickvision Parabolic", "Parabolic" },
#ifdef PORTABLE_BUILD
        m_dataFileManager{ m_appInfo.getName(), true },
#else
        m_dataFileManager{ m_appInfo.getName(), false },
#endif
        m_keyring{ m_appInfo.getId() },
        m_downloadManager{ m_dataFileManager.get<Configuration>(CONFIG_FILE_KEY).getDownloaderOptions(), m_dataFileManager.get<DownloadHistory>("history"), m_dataFileManager.get<DownloadRecoveryQueue>("recovery") },
        m_isWindowActive{ false }
    {
        m_appInfo.setVersion({ "2025.7.1" });
        m_appInfo.setShortName(_("Parabolic"));
        m_appInfo.setDescription(_("Download web video and audio"));
        m_appInfo.setChangelog("- Fixed an issue where incorrect video and audio formats were displayed for certain videos\n- Fixed an issue where Parabolic crashed when multiple downloads completed on GNOME\n- Fixed an issue where downloads were ordered wrong on Windows");
        m_appInfo.setSourceRepo("https://github.com/NickvisionApps/Parabolic");
        m_appInfo.setIssueTracker("https://github.com/NickvisionApps/Parabolic/issues/new");
        m_appInfo.setSupportUrl("https://github.com/NickvisionApps/Parabolic/discussions");
        m_appInfo.setHtmlDocsStore(m_appInfo.getVersion().getVersionType() == VersionType::Stable ? "https://github.com/NickvisionApps/Parabolic/blob/" + m_appInfo.getVersion().str() + "/docs/html" : "https://github.com/NickvisionApps/Parabolic/blob/main/docs/html");
        m_appInfo.getExtraLinks()[_("Matrix Chat")] = "https://matrix.to/#/#nickvision:matrix.org";
        m_appInfo.getDevelopers()["Nicholas Logozzo"] = "https://github.com/nlogozzo";
        m_appInfo.getDevelopers()[_("Contributors on GitHub")] = "https://github.com/NickvisionApps/Parabolic/graphs/contributors";
        m_appInfo.getDesigners()["Nicholas Logozzo"] = "https://github.com/nlogozzo";
        m_appInfo.getDesigners()[_("Fyodor Sobolev")] = "https://github.com/fsobolev";
        m_appInfo.getDesigners()["DaPigGuy"] = "https://github.com/DaPigGuy";
        m_appInfo.getArtists()[_("David Lapshin")] = "https://github.com/daudix";
        m_appInfo.setTranslatorCredits(_("translator-credits"));
        Gettext::init(m_appInfo.getEnglishShortName());
        std::string translationLanguage{ m_dataFileManager.get<Configuration>(CONFIG_FILE_KEY).getTranslationLanguage() };
        if(!translationLanguage.empty())
        {
            Gettext::changeLanguage(translationLanguage);
        }
#ifdef _WIN32
        m_updater = std::make_shared<Updater>(m_appInfo.getSourceRepo());
#endif
        m_dataFileManager.get<Configuration>(CONFIG_FILE_KEY).saved() += [this](const EventArgs&){ onConfigurationSaved(); };
        m_downloadManager.downloadCompleted() += [this](const DownloadCompletedEventArgs& args) { onDownloadCompleted(args); };
    }

    const AppInfo& MainWindowController::getAppInfo() const
    {
        return m_appInfo;
    }

    DownloadManager& MainWindowController::getDownloadManager()
    {
        return m_downloadManager;
    }

    Theme MainWindowController::getTheme()
    {
        return m_dataFileManager.get<Configuration>(CONFIG_FILE_KEY).getTheme();
    }

    void MainWindowController::setShowDisclaimerOnStartup(bool showDisclaimerOnStartup)
    {
        Configuration& config{ m_dataFileManager.get<Configuration>(CONFIG_FILE_KEY) };
        config.setShowDisclaimerOnStartup(showDisclaimerOnStartup);
        config.save();
    }

    void MainWindowController::setIsWindowActive(bool isWindowActive)
    {
        m_isWindowActive = isWindowActive;
    }

    Event<EventArgs>& MainWindowController::configurationSaved()
    {
        return m_dataFileManager.get<Configuration>(CONFIG_FILE_KEY).saved();
    }

    Event<NotificationSentEventArgs>& MainWindowController::notificationSent()
    {
        return AppNotification::sent();
    }

    std::string MainWindowController::getDebugInformation(const std::string& extraInformation) const
    {
        std::stringstream builder;
        //yt-dlp
        if(Environment::findDependency("yt-dlp").empty())
        {
            builder << "yt-dlp not found" << std::endl;
        }
        else
        {
            std::string ytdlpVersion{ Environment::exec("\"" + Environment::findDependency("yt-dlp").string() + "\"" + " --version") };
            builder << "yt-dlp version " << ytdlpVersion;
        }
        //ffmpeg
        if(Environment::findDependency("ffmpeg").empty())
        {
            builder << "ffmpeg not found" << std::endl;
        }
        else
        {
            std::string ffmpegVersion{ Environment::exec("\"" + Environment::findDependency("ffmpeg").string() + "\"" + " -version") };
            builder << ffmpegVersion.substr(0, ffmpegVersion.find("Copyright")) << std::endl;
        }
        //aria2c
        if(Environment::findDependency("aria2c").empty())
        {
            builder << "aria2c not found" << std::endl;
        }
        else
        {
            std::string aria2cVersion{ Environment::exec("\"" + Environment::findDependency("aria2c").string() + "\"" + " --version") };
            builder << aria2cVersion.substr(0, aria2cVersion.find('\n')) << std::endl;
        }
        //Extra
        if(!extraInformation.empty())
        {
            builder << std::endl << extraInformation << std::endl;
#ifdef __linux__
            builder << Environment::exec("locale");
#endif
        }
        return Environment::getDebugInformation(m_appInfo, builder.str());
    }

    bool MainWindowController::canShutdown() const
    {
        return m_downloadManager.getRemainingDownloadsCount() == 0;
    }

    std::string MainWindowController::getHelpUrl(const std::string& pageName)
    {
        return Documentation::getHelpUrl(m_appInfo.getEnglishShortName(), m_appInfo.getHtmlDocsStore(), pageName);
    }

    std::shared_ptr<AddDownloadDialogController> MainWindowController::createAddDownloadDialogController()
    {
        return std::make_shared<AddDownloadDialogController>(m_downloadManager, m_dataFileManager, m_keyring);
    }

    std::shared_ptr<CredentialDialogController> MainWindowController::createCredentialDialogController(const DownloadCredentialNeededEventArgs& args)
    {
        return std::make_shared<CredentialDialogController>(args, m_keyring);
    }

    std::shared_ptr<KeyringDialogController> MainWindowController::createKeyringDialogController()
    {
        return std::make_shared<KeyringDialogController>(m_keyring);
    }

    std::shared_ptr<PreferencesViewController> MainWindowController::createPreferencesViewController()
    {
        return std::make_shared<PreferencesViewController>(m_dataFileManager.get<Configuration>(CONFIG_FILE_KEY), m_dataFileManager.get<DownloadHistory>("history"));
    }

#ifdef _WIN32
    const StartupInformation& MainWindowController::startup(HWND hwnd)
#elif defined(__linux__)
    const StartupInformation& MainWindowController::startup(const std::string& desktopFile)
#else
    const StartupInformation& MainWindowController::startup()
#endif
    {
        static StartupInformation info;
        if (m_started)
        {
            return info;
        }
        //Load configuration
        info.setWindowGeometry(m_dataFileManager.get<Configuration>(CONFIG_FILE_KEY).getWindowGeometry());
        //Load taskbar item
#ifdef _WIN32
        m_taskbar.connect(hwnd);
        if (m_dataFileManager.get<Configuration>(CONFIG_FILE_KEY).getAutomaticallyCheckForUpdates())
        {
            checkForUpdates(false);
        }
#elif defined(__linux__)
        m_taskbar.connect(desktopFile);
#endif
        //Check if can download
        info.setCanDownload(!Environment::findDependency("yt-dlp").empty() && !Environment::findDependency("ffmpeg").empty() && !Environment::findDependency("aria2c").empty());
        //Check if disclaimer should be shown
        info.setShowDisclaimer(m_dataFileManager.get<Configuration>(CONFIG_FILE_KEY).getShowDisclaimerOnStartup());
        //Get URL to validate from args
        if(m_args.size() > 1)
        {
            if(StringHelpers::isValidUrl(m_args[1]))
            {
                info.setUrlToValidate(m_args[1]);
            }
        }
        //Load DownloadManager
        m_downloadManager.startup(info);
        m_started = true;
        return info;
    }

    void MainWindowController::shutdown(const WindowGeometry& geometry)
    {
        //Save config
        Configuration& config{ m_dataFileManager.get<Configuration>(CONFIG_FILE_KEY) };
        config.setWindowGeometry(geometry);
        config.save();
    }

    void MainWindowController::checkForUpdates(bool noUpdateNotification) const
    {
        if(!m_updater)
        {
            return;
        }
        std::thread worker{ [this, noUpdateNotification]()
        {
            Version latest{ m_updater->fetchCurrentVersion(VersionType::Stable) };
            if(!latest.empty())
            {
                if(latest > m_appInfo.getVersion())
                {
#ifdef PORTABLE_BUILD
                    AppNotification::send({ _("New update available"), NotificationSeverity::Success });
#else
                    AppNotification::send({ _("New update available"), NotificationSeverity::Success, "update" });
#endif
                    return;
                }
            }
            if(noUpdateNotification)
            {
                AppNotification::send({ _("No update available"), NotificationSeverity::Warning });
            }
        } };
        worker.detach();
    }

#ifdef _WIN32
    void MainWindowController::windowsUpdate()
    {
        if(!m_updater)
        {
            return;
        }
        AppNotification::send({ _("The update is downloading in the background and will start once it finishes"), NotificationSeverity::Informational });
        std::thread worker{ [this]()
        {
            if(!m_updater->windowsUpdate(VersionType::Stable))
            {
                AppNotification::send({ _("Unable to download and install update"), NotificationSeverity::Error, "error" });
            }
        } };
        worker.detach();
    }
#endif

    void MainWindowController::recoverDownloads()
    {
        try
        {
            size_t recoveredDownloads{ m_downloadManager.recoverDownloads() };
            if(recoveredDownloads > 0)
            {
                AppNotification::send({ _fn("Recovered {} download", "Recovered {} downloads", recoveredDownloads, recoveredDownloads), NotificationSeverity::Informational });
            }
        }
        catch(const std::exception& e)
        {
            AppNotification::send({ _f("Error attempting to recover downloads: {}", e.what()), NotificationSeverity::Error, "error" });
        }
    }

    void MainWindowController::clearRecoverableDownloads()
    {
        m_downloadManager.clearRecoverableDownloads();
    }

    void MainWindowController::onConfigurationSaved()
    {
        if(m_dataFileManager.get<Configuration>(CONFIG_FILE_KEY).getPreventSuspend())
        {
            m_suspendInhibitor.inhibit();
        }
        else
        {
            m_suspendInhibitor.uninhibit();
        }
        m_downloadManager.setDownloaderOptions(m_dataFileManager.get<Configuration>(CONFIG_FILE_KEY).getDownloaderOptions());
    }

    void MainWindowController::onDownloadCompleted(const DownloadCompletedEventArgs& args)
    {
        if(m_isWindowActive)
        {
            return;
        }
        if(args.getStatus() == DownloadStatus::Success)
        {
            ShellNotification::send({ _("Download Finished"), _f("{} has finished downloading", args.getPath().filename().string()), NotificationSeverity::Success, "open", args.getPath().string() }, m_appInfo, _("Open"));
        }
        else
        {
            ShellNotification::send({ _("Download Finished With Error"), _f("{} has finished with an error", args.getPath().filename().string()), NotificationSeverity::Error }, m_appInfo);
        }
    }
}
