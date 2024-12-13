﻿#include "controllers/mainwindowcontroller.h"
#include <format>
#include <sstream>
#include <thread>
#include <libnick/filesystem/userdirectories.h>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/documentation.h>
#include <libnick/localization/gettext.h>
#include <libnick/system/environment.h>
#include "models/configuration.h"
#include "models/downloadhistory.h"
#include "models/downloadrecoveryqueue.h"
#include "models/previousdownloadoptions.h"
#ifdef _WIN32
#include <windows.h>
#endif

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
        m_dataFileManager{ m_appInfo.getName() },
        m_logger{ UserDirectories::get(ApplicationUserDirectory::LocalData, m_appInfo.getName()) / "log.txt", Logging::LogLevel::Info, false },
        m_keyring{ m_appInfo.getId() },
        m_downloadManager{ m_dataFileManager.get<Configuration>("config").getDownloaderOptions(), m_dataFileManager.get<DownloadHistory>("history"), m_dataFileManager.get<DownloadRecoveryQueue>("recovery"), m_logger }
    {
        m_appInfo.setVersion({ "2024.12.0-next" });
        m_appInfo.setShortName(_("Parabolic"));
        m_appInfo.setDescription(_("Download web video and audio"));
        m_appInfo.setChangelog("- Added the ability to toggle the inclusion of a media's id in its title when validated in the app's settings\n- Added the option to export a download's media description to a separate file\n- Restored the ability for Parabolic to accept a URL to validate via command line arguments\n- Fixed an issue where auto-generated subtitles were not being embed in a media file\n- Fixed an issue where downloading media at certain time frames were not respected\n- Fixed an issue where video medias' thumbnails were also cropped when crop audio thumbnails was enabled\n- Fixed an issue where the previously used download quality was not remembered\n- Redesigned the Qt version's user interface with a more modern style\n- Updated yt-dlp to 2024.12.06");
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
        Localization::Gettext::init(m_appInfo.getEnglishShortName());
#ifdef _WIN32
        m_updater = std::make_shared<Updater>(m_appInfo.getSourceRepo());
#endif
        m_dataFileManager.get<Configuration>("config").saved() += [this](const EventArgs&){ onConfigurationSaved(); };
        //Log information
        if(!m_keyring.isSavingToDisk())
        {
            m_logger.log(Logging::LogLevel::Warning, "Keyring not being saved to disk.");
        }
        if(m_dataFileManager.get<Configuration>("config").getPreventSuspend())
        {
            if(m_suspendInhibitor.inhibit())
            {
                m_logger.log(Logging::LogLevel::Info, "Inhibited system suspend.");
            }
            else
            {
                m_logger.log(Logging::LogLevel::Error, "Unable to inhibit system suspend.");
            }
        }
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
        return m_dataFileManager.get<Configuration>("config").getTheme();
    }

    void MainWindowController::setShowDisclaimerOnStartup(bool showDisclaimerOnStartup)
    {
        Configuration& config{ m_dataFileManager.get<Configuration>("config") };
        config.setShowDisclaimerOnStartup(showDisclaimerOnStartup);
        config.save();
    }

    Event<EventArgs>& MainWindowController::configurationSaved()
    {
        return m_dataFileManager.get<Configuration>("config").saved();
    }

    Event<NotificationSentEventArgs>& MainWindowController::notificationSent()
    {
        return m_notificationSent;
    }

    Event<ShellNotificationSentEventArgs>& MainWindowController::shellNotificationSent()
    {
        return m_shellNotificationSent;
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
            std::string ytdlpVersion{ Environment::exec(Environment::findDependency("yt-dlp").string() + " --version") };
            builder << "yt-dlp version " << ytdlpVersion;
        }
        //ffmpeg
        if(Environment::findDependency("ffmpeg").empty())
        {
            builder << "ffmpeg not found" << std::endl;
        }
        else
        {
            std::string ffmpegVersion{ Environment::exec(Environment::findDependency("ffmpeg").string() + " -version") };
            builder << ffmpegVersion.substr(0, ffmpegVersion.find("Copyright")) << std::endl;
        }
        //aria2c
        if(Environment::findDependency("aria2c").empty())
        {
            builder << "aria2c not found" << std::endl;
        }
        else
        {
            std::string aria2cVersion{ Environment::exec(Environment::findDependency("aria2c").string() + " --version") };
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
        return std::make_shared<PreferencesViewController>(m_dataFileManager.get<Configuration>("config"), m_dataFileManager.get<DownloadHistory>("history"));
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
        info.setWindowGeometry(m_dataFileManager.get<Configuration>("config").getWindowGeometry());
        //Load taskbar item
#ifdef _WIN32
        if(m_taskbar.connect(hwnd))
        {
            m_logger.log(Logging::LogLevel::Info, "Connected to Windows taskbar.");
        }
        else
        {
            m_logger.log(Logging::LogLevel::Error, "Unable to connect to Windows taskbar.");
        }
        if (m_dataFileManager.get<Configuration>("config").getAutomaticallyCheckForUpdates())
        {
            checkForUpdates();
        }
#elif defined(__linux__)
        if(m_taskbar.connect(desktopFile))
        {
            m_logger.log(Logging::LogLevel::Info, "Connected to Linux taskbar.");
        }
        else
        {
            m_logger.log(Logging::LogLevel::Error, "Unable to connect to Linux taskbar.");
        }
#endif
        //Check if can download
        if(Environment::findDependency("yt-dlp").empty())
        {
            m_logger.log(Logging::LogLevel::Error, "yt-dlp not found.");
        }
        if(Environment::findDependency("ffmpeg").empty())
        {
            m_logger.log(Logging::LogLevel::Error, "ffmpeg not found.");
        }
        if(Environment::findDependency("aria2c").empty())
        {
            m_logger.log(Logging::LogLevel::Error, "aria2c not found.");
        }
        info.setCanDownload(!Environment::findDependency("yt-dlp").empty() && !Environment::findDependency("ffmpeg").empty() && !Environment::findDependency("aria2c").empty());
        //Check if disclaimer should be shown
        info.setShowDisclaimer(m_dataFileManager.get<Configuration>("config").getShowDisclaimerOnStartup());
        //Get URL to validate from args
        if(m_args.size() > 1)
        {
            if(StringHelpers::isValidUrl(m_args[1]))
            {
                info.setUrlToValidate(m_args[1]);
            }
        }
        //Load DownloadManager
        size_t recoveredDownloads{ m_downloadManager.startup(m_dataFileManager.get<Configuration>("config").getRecoverCrashedDownloads()) };
        if(recoveredDownloads > 0)
        {
            m_notificationSent.invoke({ std::vformat(_n("Recovered {} download", "Recovered {} downloads", recoveredDownloads), std::make_format_args(recoveredDownloads)), NotificationSeverity::Informational });
        }
        m_started = true;
        return info;
    }

    void MainWindowController::shutdown(const WindowGeometry& geometry)
    {
        //Save config
        Configuration& config{ m_dataFileManager.get<Configuration>("config") };
        config.setWindowGeometry(geometry);
        config.save();
    }

    void MainWindowController::checkForUpdates()
    {
        if(!m_updater)
        {
            return;
        }
        m_logger.log(Logging::LogLevel::Info, "Checking for updates...");
        std::thread worker{ [this]()
        {
            Version latest{ m_updater->fetchCurrentVersion(VersionType::Stable) };
            if (!latest.empty())
            {
                if (latest > m_appInfo.getVersion())
                {
                    m_logger.log(Logging::LogLevel::Info, "Update found: " + latest.str());
                    m_notificationSent.invoke({ _("New update available"), NotificationSeverity::Success, "update" });
                }
                else
                {
                    m_logger.log(Logging::LogLevel::Info, "No updates found.");
                }
            }
            else
            {
                m_logger.log(Logging::LogLevel::Warning, "Unable to fetch latest app version.");
            }
        } };
        worker.detach();
    }

#ifdef _WIN32
    void MainWindowController::windowsUpdate()
    {
        if(m_updater)
        {
            return;
        }
        m_logger.log(Logging::LogLevel::Info, "Fetching Windows app update...");
        std::thread worker{ [this]()
        {
            if (m_updater->windowsUpdate(VersionType::Stable))
            {
                m_logger.log(Logging::LogLevel::Info, "Windows app update started.");
            }
            else
            {
                m_logger.log(Logging::LogLevel::Error, "Unable to fetch Windows app update.");
                m_notificationSent.invoke({ _("Unable to download and install update"), NotificationSeverity::Error, "error" });
            }
        } };
        worker.detach();
    }
#endif

    void MainWindowController::log(Logging::LogLevel level, const std::string& message, const std::source_location& source)
    {
        m_logger.log(level, message, source);
    }

    void MainWindowController::onConfigurationSaved()
    {
        m_logger.log(Logging::LogLevel::Info, "Configuration saved.");
        if(m_dataFileManager.get<Configuration>("config").getPreventSuspend())
        {
            if(m_suspendInhibitor.inhibit())
            {
                m_logger.log(Logging::LogLevel::Info, "Inhibited system suspend.");
            }
            else
            {
                m_logger.log(Logging::LogLevel::Error, "Unable to inhibit system suspend.");
            }
        }
        else
        {
            if(m_suspendInhibitor.uninhibit())
            {
                m_logger.log(Logging::LogLevel::Info, "Uninhibited system suspend.");
            }
            else
            {
                m_logger.log(Logging::LogLevel::Error, "Unable to uninhibit system suspend.");
            }
        }
        m_downloadManager.setDownloaderOptions(m_dataFileManager.get<Configuration>("config").getDownloaderOptions());
    }
}
