#include "controllers/mainwindowcontroller.h"
#include <algorithm>
#include <ctime>
#include <format>
#include <sstream>
#include <thread>
#include <libnick/filesystem/userdirectories.h>
#include <libnick/helpers/codehelpers.h>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/documentation.h>
#include <libnick/localization/gettext.h>
#include <libnick/system/environment.h>
#include "helpers/pythonhelpers.h"
#include "models/configuration.h"
#include "models/downloadhistory.h"
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
using namespace Nickvision::TubeConverter::Shared::Helpers;
using namespace Nickvision::TubeConverter::Shared::Models;
using namespace Nickvision::Update;

namespace Nickvision::TubeConverter::Shared::Controllers
{
    MainWindowController::MainWindowController(const std::vector<std::string>& args)
        : m_started{ false },
        m_args{ args },
        m_appInfo{ "org.nickvision.tubeconverter", "Nickvision Parabolic", "Parabolic" },
        m_dataFileManager{ m_appInfo.getName() },
        m_logger{ UserDirectories::get(ApplicationUserDirectory::LocalData, m_appInfo.getName()) / "log.txt", std::find(m_args.begin(), m_args.end(), "--debug") != m_args.end() ? Logging::LogLevel::Debug : Logging::LogLevel::Info, false },
        m_keyring{ Keyring::Keyring::access(m_appInfo.getId()) }
    {
        m_appInfo.setVersion({ "2024.7.0-next" });
        m_appInfo.setShortName(_("Parabolic"));
        m_appInfo.setDescription(_("Download web video and audio"));
        m_appInfo.setChangelog("- Parabolic has been rewritten in C++ for faster performance\n- The length of the download history can now be changed\n- Redesigned user interface\n- Updated yt-dlp");
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
        m_dataFileManager.get<Configuration>("config").saved() += [this](const EventArgs&)
        {
            m_logger.log(Logging::LogLevel::Debug, "Configuration saved.");
        };
        m_networkMonitor.stateChanged() += [this](const NetworkStateChangedEventArgs& args)
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
        };
        if(m_keyring)
        {
            m_logger.log(Logging::LogLevel::Info, "Keyring unlocked.");
        }
        else
        {
            m_logger.log(Logging::LogLevel::Error, "Unable to unlock keyring.");
        }
    }

    const AppInfo& MainWindowController::getAppInfo() const
    {
        return m_appInfo;
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

    Event<ParamEventArgs<std::vector<HistoricDownload>>>& MainWindowController::historyChanged()
    {
        return m_historyChanged;
    }

    std::string MainWindowController::getDebugInformation(const std::string& extraInformation) const
    {
        std::stringstream builder;
        //Python
        builder << PythonHelpers::getDebugInformation() << std::endl;
        //Ffmpeg
        std::string ffmpegVersion{ Environment::exec(Environment::findDependency("ffmpeg").string() + " -version") };
        ffmpegVersion = ffmpegVersion.substr(0, ffmpegVersion.find("Copyright"));
        builder << ffmpegVersion << std::endl;
        //Aria2c
        std::string aria2cVersion{ Environment::exec(Environment::findDependency("aria2c").string() + " --version") };
        aria2cVersion = aria2cVersion.substr(0, aria2cVersion.find('\n'));
        builder << aria2cVersion << std::endl;
        //Extra
        if(!extraInformation.empty())
        {
            builder << std::endl << extraInformation;
        }
        return Environment::getDebugInformation(m_appInfo, builder.str());
    }

    bool MainWindowController::canShutdown() const
    {
        return true;
    }

    std::string MainWindowController::getHelpUrl(const std::string& pageName)
    {
        return Documentation::getHelpUrl(m_appInfo.getEnglishShortName(), m_appInfo.getHtmlDocsStore(), pageName);
    }

    bool MainWindowController::canDownload() const
    {
        return m_networkMonitor.getConnectionState() == NetworkState::ConnectedGlobal && PythonHelpers::started();
    }

    std::shared_ptr<PreferencesViewController> MainWindowController::createPreferencesViewController()
    {
        return std::make_shared<PreferencesViewController>(m_dataFileManager.get<Configuration>("config"));
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
            m_logger.log(Logging::LogLevel::Debug, "Connected to Windows taskbar.");
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
            m_logger.log(Logging::LogLevel::Debug, "Connected to Linux taskbar.");
        }
        else
        {
            m_logger.log(Logging::LogLevel::Error, "Unable to connect to Linux taskbar.");
        }
#endif
        //Start python
        PythonHelpers::start(m_logger);
        //Load history
        m_logger.log(Logging::LogLevel::Debug, "Loading historic downloads...");
        DownloadHistory& history{ m_dataFileManager.get<DownloadHistory>("history") };
        m_logger.log(Logging::LogLevel::Info, "Loaded " + std::to_string(history.getHistory().size()) + " historic downloads.");
        m_historyChanged.invoke(history.getHistory());
        //Check if disclaimer should be shown
        if(m_dataFileManager.get<Configuration>("config").getShowDisclaimerOnStartup())
        {
            m_disclaimerTriggered.invoke({ _("The authors of Nickvision Parabolic are not responsible/liable for any misuse of this program that may violate local copyright/DMCA laws. Users use this application at their own risk.") });
        }
        //Check if downloads can be started
        m_downloadAbilityChanged.invoke(canDownload());
        m_started = true;
        m_logger.log(Logging::LogLevel::Debug, "MainWindow started.");
        return m_dataFileManager.get<Configuration>("config").getWindowGeometry();
    }

    void MainWindowController::shutdown(const WindowGeometry& geometry)
    {
        //Shutdown python
        PythonHelpers::shutdown(m_logger);
        //Save config
        Configuration& config{ m_dataFileManager.get<Configuration>("config") };
        config.setWindowGeometry(geometry);
        config.save();
        m_logger.log(Logging::LogLevel::Debug, "MainWindow shutdown.");
    }

    void MainWindowController::checkForUpdates()
    {
        if(!m_updater)
        {
            return;
        }
        m_logger.log(Logging::LogLevel::Debug, "Checking for updates...");
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
                    m_logger.log(Logging::LogLevel::Debug, "No updates found.");
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
        m_logger.log(Logging::LogLevel::Debug, "Fetching Windows app update...");
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

    void MainWindowController::clearHistory()
    {
        DownloadHistory& history{ m_dataFileManager.get<DownloadHistory>("history") };
        if(history.clear())
        {
            m_historyChanged.invoke(history.getHistory());
        }
    }

    void MainWindowController::removeHistoricDownload(const HistoricDownload& download)
    {
        DownloadHistory& history{ m_dataFileManager.get<DownloadHistory>("history") };
        if(history.removeDownload(download))
        {
            m_historyChanged.invoke(history.getHistory());
        }
    }
}
