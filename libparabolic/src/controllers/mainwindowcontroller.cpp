#include "controllers/mainwindowcontroller.h"
#include <algorithm>
#include <ctime>
#include <format>
#include <locale>
#include <sstream>
#include <thread>
#include <libnick/app/aura.h>
#include <libnick/helpers/codehelpers.h>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/gettext.h>
#include <pybind11/embed.h>
#include "models/configuration.h"
#include "models/downloadhistory.h"
#ifdef _WIN32
#include <windows.h>
#endif

using namespace Nickvision::App;
using namespace Nickvision::Events;
using namespace Nickvision::Helpers;
using namespace Nickvision::Network;
using namespace Nickvision::Notifications;
using namespace Nickvision::TubeConverter::Shared::Models;
using namespace Nickvision::Update;
namespace py = pybind11;

namespace Nickvision::TubeConverter::Shared::Controllers
{
    MainWindowController::MainWindowController(const std::vector<std::string>& args)
        : m_started{ false },
        m_args{ args }
    {
        Logging::LogLevel logLevel{ std::find(m_args.begin(), m_args.end(), "--debug") != m_args.end() ? Logging::LogLevel::Debug : Logging::LogLevel::Info };
        Aura::getActive().init("org.nickvision.tubeconverter", "Nickvision Parabolic", "Parabolic", logLevel);
        AppInfo& appInfo{ Aura::getActive().getAppInfo() };
        appInfo.setVersion({ "2024.6.0-next" });
        appInfo.setShortName(_("Parabolic"));
        appInfo.setDescription(_("Download web video and audio"));
        appInfo.setSourceRepo("https://github.com/NickvisionApps/Parabolic");
        appInfo.setIssueTracker("https://github.com/NickvisionApps/Parabolic/issues/new");
        appInfo.setSupportUrl("https://github.com/NickvisionApps/Parabolic/discussions");
        appInfo.getExtraLinks()[_("Matrix Chat")] = "https://matrix.to/#/#nickvision:matrix.org";
        appInfo.getDevelopers()["Nicholas Logozzo"] = "https://github.com/nlogozzo";
        appInfo.getDevelopers()[_("Contributors on GitHub")] = "https://github.com/NickvisionApps/Parabolic/graphs/contributors";
        appInfo.getDesigners()["Nicholas Logozzo"] = "https://github.com/nlogozzo";
        appInfo.getDesigners()[_("Fyodor Sobolev")] = "https://github.com/fsobolev";
        appInfo.getDesigners()["DaPigGuy"] = "https://github.com/DaPigGuy";
        appInfo.getArtists()[_("David Lapshin")] = "https://github.com/daudix";
        appInfo.setTranslatorCredits(_("translator-credits"));
        m_networkMonitor.stateChanged() += [&](const NetworkStateChangedEventArgs& args){ onNetworkChanged(args); };
    }

    AppInfo& MainWindowController::getAppInfo() const
    {
        return Aura::getActive().getAppInfo();
    }

    bool MainWindowController::isDevVersion() const
    {
        return Aura::getActive().getAppInfo().getVersion().getVersionType() == VersionType::Preview;
    }

    Theme MainWindowController::getTheme() const
    {
        return Aura::getActive().getConfig<Configuration>("config").getTheme();
    }

    WindowGeometry MainWindowController::getWindowGeometry() const
    {
        return Aura::getActive().getConfig<Configuration>("config").getWindowGeometry();
    }
    void MainWindowController::setShowDisclaimerOnStartup(bool showDisclaimerOnStartup)
    {
        Configuration& config{ Aura::getActive().getConfig<Configuration>("config") };
        config.setShowDisclaimerOnStartup(showDisclaimerOnStartup);
        config.save();
    }

    Event<EventArgs>& MainWindowController::configurationSaved()
    {
        return Aura::getActive().getConfig<Configuration>("config").saved();
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

    Event<ParamEventArgs<std::vector<HistoricDownload>>>& MainWindowController::historyChanged()
    {
        return m_historyChanged;
    }

    std::string MainWindowController::getDebugInformation(const std::string& extraInformation) const
    {
        std::stringstream builder;
        builder << Aura::getActive().getAppInfo().getId();
#ifdef _WIN32
        builder << ".winui" << std::endl;
#elif defined(__linux__)
        builder << ".gnome" << std::endl;
#endif
        builder << Aura::getActive().getAppInfo().getVersion().str() << std::endl << std::endl;
        if(Aura::getActive().isRunningViaFlatpak())
        {
            builder << "Running under Flatpak" << std::endl;
        }
        else if(Aura::getActive().isRunningViaSnap())
        {
            builder << "Running under Snap" << std::endl;
        }
        else
        {
            builder << "Running locally" << std::endl;
        }
#ifdef _WIN32
        LCID lcid = GetThreadLocale();
        wchar_t name[LOCALE_NAME_MAX_LENGTH];
        if(LCIDToLocaleName(lcid, name, LOCALE_NAME_MAX_LENGTH, 0) > 0)
        {
            builder << StringHelpers::str(name) << std::endl;
        }
#elif defined(__linux__)
        try
        {
            builder << std::locale("").name() << std::endl;
        }
        catch(...)
        {
            builder << "Locale not set" << std::endl;
        }
#endif
        if (!extraInformation.empty())
        {
            builder << extraInformation << std::endl;
        }
        return builder.str();
    }

    std::shared_ptr<PreferencesViewController> MainWindowController::createPreferencesViewController() const
    {
        return std::make_shared<PreferencesViewController>();
    }

    void MainWindowController::startup()
    {
        if (m_started)
        {
            return;
        }
#ifdef _WIN32
        try
        {
            m_updater = std::make_shared<Updater>(Aura::getActive().getAppInfo().getSourceRepo());
        }
        catch(...)
        {
            m_updater = nullptr;
        }
        if (Aura::getActive().getConfig<Configuration>("config").getAutomaticallyCheckForUpdates())
        {
            checkForUpdates();
        }
#endif
        //Check if disclaimer should be shown
        if(Aura::getActive().getConfig<Configuration>("config").getShowDisclaimerOnStartup())
        {
            m_disclaimerTriggered.invoke({ _("The authors of Nickvision Parabolic are not responsible/liable for any misuse of this program that may violate local copyright/DMCA laws. Users use this application at their own risk.") });
        }
        //Load history
        Aura::getActive().getLogger().log(Logging::LogLevel::Debug, "Loading historic downloads...");
        DownloadHistory& history{ Aura::getActive().getConfig<DownloadHistory>("history") };
        Aura::getActive().getLogger().log(Logging::LogLevel::Info, "Loaded " + std::to_string(history.getHistory().size()) + " historic downloads.");
        m_historyChanged.invoke(history.getHistory());
        //Check network
        onNetworkChanged({ m_networkMonitor.getConnectionState() });
        //Load python
        try
        {
            Aura::getActive().getLogger().log(Logging::LogLevel::Info, "Loading python...");
            py::initialize_interpreter();
            Aura::getActive().getLogger().log(Logging::LogLevel::Info, "Loading yt-dlp...");
            py::module_ ytdlp{ py::module_::import("yt_dlp") };
            Aura::getActive().getLogger().log(Logging::LogLevel::Info, "Python loaded.");
        }
        catch(const std::exception& e)
        {
            Aura::getActive().getLogger().log(Logging::LogLevel::Error, "Unable to load python: " + std::string(e.what()));
            m_notificationSent.invoke({ _("Unable to load python"), NotificationSeverity::Error, "nopython" });
        }
        m_started = true;
        Aura::getActive().getLogger().log(Logging::LogLevel::Debug, "MainWindow started.");
    }

    void MainWindowController::shutdown(const WindowGeometry& geometry)
    {
        //Save config
        Configuration& config{ Aura::getActive().getConfig<Configuration>("config") };
        config.setWindowGeometry(geometry);
        config.save();
        //Shutdown python
        py::finalize_interpreter();
        Aura::getActive().getLogger().log(Logging::LogLevel::Debug, "MainWindow shutdown.");
    }

    void MainWindowController::checkForUpdates()
    {
        if(!m_updater)
        {
            return;
        }
        Aura::getActive().getLogger().log(Logging::LogLevel::Debug, "Checking for updates...");
        std::thread worker{ [this]()
        {
            Version latest{ m_updater->fetchCurrentVersion(VersionType::Stable) };
            if (!latest.empty())
            {
                if (latest > Aura::getActive().getAppInfo().getVersion())
                {
                    Aura::getActive().getLogger().log(Logging::LogLevel::Info, "Update found: " + latest.str());
                    m_notificationSent.invoke({ _("New update available"), NotificationSeverity::Success, "update" });
                }
                else
                {
                    Aura::getActive().getLogger().log(Logging::LogLevel::Debug, "No updates found.");
                }
            }
            else
            {
                Aura::getActive().getLogger().log(Logging::LogLevel::Warning, "Unable to fetch latest app version.");
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
        Aura::getActive().getLogger().log(Logging::LogLevel::Debug, "Fetching Windows app update...");
        std::thread worker{ [this]()
        {
            if (m_updater->windowsUpdate(VersionType::Stable))
            {
                Aura::getActive().getLogger().log(Logging::LogLevel::Info, "Windows app update started.");
            }
            else
            {
                Aura::getActive().getLogger().log(Logging::LogLevel::Error, "Unable to fetch Windows app update.");
                m_notificationSent.invoke({ _("Unable to download and install update"), NotificationSeverity::Error, "error" });
            }
        } };
        worker.detach();
    }

    void MainWindowController::connectTaskbar(HWND hwnd)
    {
        if(m_taskbar.connect(hwnd))
        {
            Aura::getActive().getLogger().log(Logging::LogLevel::Debug, "Connected to Windows taskbar.");
        }
        else
        {
            Aura::getActive().getLogger().log(Logging::LogLevel::Error, "Unable to connect to Windows taskbar.");
        }
    }
#elif defined(__linux__)
    void MainWindowController::connectTaskbar(const std::string& desktopFile)
    {
        if(m_taskbar.connect(desktopFile))
        {
            Aura::getActive().getLogger().log(Logging::LogLevel::Debug, "Connected to Linux taskbar.");
        }
        else
        {
            Aura::getActive().getLogger().log(Logging::LogLevel::Error, "Unable to connect to Linux taskbar.");
        }
    }
#endif

    void MainWindowController::clearHistory()
    {
        DownloadHistory& history{ Aura::getActive().getConfig<DownloadHistory>("history") };
        if(history.clear())
        {
            m_historyChanged.invoke(history.getHistory());
        }
    }

    void MainWindowController::removeHistoricDownload(const HistoricDownload& download)
    {
        DownloadHistory& history{ Aura::getActive().getConfig<DownloadHistory>("history") };
        if(history.removeDownload(download))
        {
            m_historyChanged.invoke(history.getHistory());
        }
    }

    void MainWindowController::onNetworkChanged(const NetworkStateChangedEventArgs& args)
    {
        if(args.getState() == NetworkState::ConnectedGlobal)
        {
            Aura::getActive().getLogger().log(Logging::LogLevel::Info, "Network connected.");
            m_notificationSent.invoke({ "", NotificationSeverity::Informational, "network" });
        }
        else
        {
            Aura::getActive().getLogger().log(Logging::LogLevel::Info, "Network disconnected.");
            m_notificationSent.invoke({ _("No network connection"), NotificationSeverity::Error, "nonetwork" });
        }
    }
}
