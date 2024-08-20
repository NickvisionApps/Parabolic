#include "controllers/mainwindowcontroller.h"
#include <format>
#include <sstream>
#include <thread>
#include <libnick/filesystem/userdirectories.h>
#include <libnick/helpers/codehelpers.h>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/documentation.h>
#include <libnick/localization/gettext.h>
#include <libnick/system/environment.h>
#include "models/configuration.h"
#include "models/downloadhistory.h"
#include "models/previousdownloadoptions.h"
#ifdef _WIN32
#include <windows.h>
#endif

using namespace Nickvision::App;
using namespace Nickvision::Events;
using namespace Nickvision::Filesystem;
using namespace Nickvision::Helpers;
using namespace Nickvision::Keyring;
using namespace Nickvision::Network;
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
        m_downloadManager{ m_dataFileManager.get<Configuration>("config").getDownloaderOptions(), m_dataFileManager.get<DownloadHistory>("history"), m_logger }
    {
        m_appInfo.setVersion({ "2024.8.0-next" });
        m_appInfo.setShortName(_("Parabolic"));
        m_appInfo.setDescription(_("Download web video and audio"));
        m_appInfo.setChangelog("- Parabolic has been rewritten in C++ for faster performance\n- The length of the kept download history can now be changed in the app's preferences\n- Cookies will now be fetched from a selected browser in the app's preferences.\nTXT cookies file uploads are no longer supported\n- Parabolic's Keyring module was rewritten.nAs a result, all keyrings have been reset and will need to be reconfigured.\n- Redesigned user interface\n- Updated yt-dlp");
        m_appInfo.setSourceRepo("https://github.com/NickvisionApps/Parabolic");
        m_appInfo.setIssueTracker("https://github.com/NickvisionApps/Parabolic/issues/new");
        m_appInfo.setSupportUrl("https://github.com/NickvisionApps/Parabolic/discussions");
        m_appInfo.setHtmlDocsStore("https://github.com/NickvisionApps/Parabolic/blob/main/NickvisionTubeConverter.Shared/Docs/html");
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
        m_networkMonitor.stateChanged() += [this](const NetworkStateChangedEventArgs& args){ onNetworkStateChanged(args); };
        if(!m_keyring.isSavingToDisk())
        {
            m_logger.log(Logging::LogLevel::Warning, "Keyring not being saved to disk.");
        }
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

    Event<ParamEventArgs<std::string>>& MainWindowController::disclaimerTriggered()
    {
        return m_disclaimerTriggered;
    }

    Event<ParamEventArgs<bool>>& MainWindowController::downloadAbilityChanged()
    {
        return m_downloadAbilityChanged;
    }

    std::string MainWindowController::getDebugInformation(const std::string& extraInformation) const
    {
        std::stringstream builder;
        //Network connection
        builder << "Network: ";
        switch(m_networkMonitor.getConnectionState())
        {
        case NetworkState::Disconnected:
            builder << "Disconnected" << std::endl;
            break;
        case NetworkState::ConnectedLocal:
            builder << "Local" << std::endl;
            break;
        case NetworkState::ConnectedGlobal:
            builder << "Global" << std::endl;
            break;
        }
        builder << std::endl;
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
            builder << std::endl << extraInformation;
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

    bool MainWindowController::canDownload() const
    {
        return m_networkMonitor.getConnectionState() == NetworkState::ConnectedGlobal && !Environment::findDependency("yt-dlp").empty() && !Environment::findDependency("ffmpeg").empty() && !Environment::findDependency("aria2c").empty();
    }

    std::shared_ptr<AddDownloadDialogController> MainWindowController::createAddDownloadDialogController()
    {
        return std::make_shared<AddDownloadDialogController>(m_downloadManager, m_dataFileManager.get<PreviousDownloadOptions>("prev"), m_keyring);
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
    Nickvision::App::WindowGeometry MainWindowController::startup(HWND hwnd)
#elif defined(__linux__)
    Nickvision::App::WindowGeometry MainWindowController::startup(const std::string& desktopFile)
#endif
    {
        if (m_started)
        {
            return m_dataFileManager.get<Configuration>("config").getWindowGeometry();
        }
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
        //Load history
        m_downloadManager.loadHistory();
        //Check if disclaimer should be shown
        if(m_dataFileManager.get<Configuration>("config").getShowDisclaimerOnStartup())
        {
            m_disclaimerTriggered.invoke({ _("The authors of Nickvision Parabolic are not responsible/liable for any misuse of this program that may violate local copyright/DMCA laws. Users use this application at their own risk.") });
        }
        //Check if downloads can be started
        m_downloadAbilityChanged.invoke(canDownload());
        m_started = true;
        return m_dataFileManager.get<Configuration>("config").getWindowGeometry();
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

    void MainWindowController::onNetworkStateChanged(const NetworkStateChangedEventArgs& args)
    {
        if(args.getState() == NetworkState::ConnectedGlobal)
        {
            m_logger.log(Logging::LogLevel::Info, "Network connected.");
        }
        else
        {
            m_logger.log(Logging::LogLevel::Info, "Network disconnected.");
        }
        m_downloadAbilityChanged.invoke(canDownload());
    }
}
