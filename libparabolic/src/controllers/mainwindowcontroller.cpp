#include "controllers/mainwindowcontroller.h"
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
#define HISTORY_FILE_KEY "history"
#define RECOVERY_FILE_KEY "recovery"

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
        m_downloadManager{ m_dataFileManager.get<Configuration>(CONFIG_FILE_KEY), m_dataFileManager.get<DownloadHistory>(HISTORY_FILE_KEY), m_dataFileManager.get<DownloadRecoveryQueue>(RECOVERY_FILE_KEY) },
        m_isWindowActive{ false }
    {
        m_appInfo.setVersion({ "2025.8.0-beta2" });
        m_appInfo.setShortName(_("Parabolic"));
        m_appInfo.setDescription(_("Download web video and audio"));
        m_appInfo.setChangelog("- Added the ability to update yt-dlp from within the app when a newer version is available\n- Replaced None translation language with en_US\n- Fixed an issue where validating some media would cause the app to crash\n- Fixed an issue where the app would not open on Windows\n- Fixed an issue where download rows disappeared on GNOME\n- Updated yt-dlp");
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
        m_updater = std::make_shared<Updater>(m_appInfo.getSourceRepo());
        m_dataFileManager.get<Configuration>(CONFIG_FILE_KEY).saved() += [this](const EventArgs&){ onConfigurationSaved(); };
        m_downloadManager.downloadCompleted() += [this](const DownloadCompletedEventArgs& args) { onDownloadCompleted(args); };
    }

    const AppInfo& MainWindowController::getAppInfo() const
    {
        return m_appInfo;
    }

    Theme MainWindowController::getTheme()
    {
        return m_dataFileManager.get<Configuration>(CONFIG_FILE_KEY).getTheme();
    }

    VersionType MainWindowController::getPreferredUpdateType()
    {
        return m_dataFileManager.get<Configuration>(CONFIG_FILE_KEY).getPreferredUpdateType();
    }

    void MainWindowController::setPreferredUpdateType(VersionType type)
    {
        m_dataFileManager.get<Configuration>(CONFIG_FILE_KEY).setPreferredUpdateType(type);
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

    Event<ParamEventArgs<Version>>& MainWindowController::appUpdateAvailable()
    {
        return m_appUpdateAvailable;
    }

    Event<ParamEventArgs<double>>& MainWindowController::appUpdateProgressChanged()
    {
        return m_appUpdateProgressChanged;
    }

    Event<ParamEventArgs<std::vector<HistoricDownload>>>& MainWindowController::historyChanged()
    {
        return m_downloadManager.historyChanged();
    }

    Event<DownloadAddedEventArgs>& MainWindowController::downloadAdded()
    {
        return m_downloadManager.downloadAdded();
    }

    Event<DownloadCompletedEventArgs>& MainWindowController::downloadCompleted()
    {
        return m_downloadManager.downloadCompleted();
    }

    Event<DownloadProgressChangedEventArgs>& MainWindowController::downloadProgressChanged()
    {
        return m_downloadManager.downloadProgressChanged();
    }

    Event<ParamEventArgs<int>>& MainWindowController::downloadStopped()
    {
        return m_downloadManager.downloadStopped();
    }

    Event<ParamEventArgs<int>>& MainWindowController::downloadPaused()
    {
        return m_downloadManager.downloadPaused();
    }

    Event<ParamEventArgs<int>>& MainWindowController::downloadResumed()
    {
        return m_downloadManager.downloadResumed();
    }

    Event<ParamEventArgs<int>>& MainWindowController::downloadRetried()
    {
        return m_downloadManager.downloadRetried();
    }

    Event<ParamEventArgs<int>>& MainWindowController::downloadStartedFromQueue()
    {
        return m_downloadManager.downloadStartedFromQueue();
    }

    Event<DownloadCredentialNeededEventArgs>& MainWindowController::downloadCredentialNeeded()
    {
        return m_downloadManager.downloadCredentialNeeded();
    }

    std::string MainWindowController::getDebugInformation(const std::string& extraInformation) const
    {
        std::stringstream builder;
        //yt-dlp
        if(m_downloadManager.getYtdlpExecutablePath().empty())
        {
            builder << "yt-dlp not found" << std::endl;
        }
        else
        {
            std::string ytdlpVersion{ Environment::exec("\"" + m_downloadManager.getYtdlpExecutablePath().string() + "\"" + " --version") };
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
        return std::make_shared<PreferencesViewController>(m_dataFileManager.get<Configuration>(CONFIG_FILE_KEY), m_dataFileManager.get<DownloadHistory>(HISTORY_FILE_KEY));
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
        WindowGeometry geometry{ m_dataFileManager.get<Configuration>(CONFIG_FILE_KEY).getWindowGeometry() };
        if(geometry.getX() < 0 || geometry.getY() < 0)
        {
            geometry.setX(10);
            geometry.setY(10);
        }
        info.setWindowGeometry(geometry);

        //Load taskbar item
#ifdef _WIN32
        m_taskbar.connect(hwnd);
#elif defined(__linux__)
        m_taskbar.connect(desktopFile);
#endif
        //Start checking for app updates
        std::thread workerUpdates{ [this]()
        {
            Version latest{ m_updater->fetchCurrentVersion(getPreferredUpdateType()) };
            if(!latest.empty())
            {
                if(latest > m_appInfo.getVersion())
                {
                    m_appUpdateAvailable.invoke({ latest });
                }
            }
        } };
        workerUpdates.detach();
        //Load DownloadManager
        m_downloadManager.startup(info);
        //Check if can download
        info.setCanDownload(!m_downloadManager.getYtdlpExecutablePath().empty() && !Environment::findDependency("ffmpeg").empty() && !Environment::findDependency("aria2c").empty());
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

#ifdef _WIN32
    void MainWindowController::startWindowsUpdate()
    {
        std::thread worker{ [this]()
        {
            m_appUpdateProgressChanged.invoke({ 0.0 });
            bool res{ m_updater->windowsUpdate(getPreferredUpdateType(), { [this](curl_off_t downloadTotal, curl_off_t downloadNow, curl_off_t, curl_off_t, intptr_t) -> bool
            {
                if(downloadTotal == 0)
                {
                    return true;
                }
                m_appUpdateProgressChanged.invoke({ static_cast<double>(static_cast<long double>(downloadNow) / static_cast<long double>(downloadTotal)) });
                return true;
            } }) };
            if(res)
            {
                m_appUpdateProgressChanged.invoke({ 1.0 });
            }
            else
            {
                AppNotification::send({ _("Unable to download and install update"), NotificationSeverity::Error });
            }
        } };
        worker.detach();
    }
#endif

    void MainWindowController::ytdlpUpdate()
    {
        m_downloadManager.ytdlpUpdate();
    }

    size_t MainWindowController::getRemainingDownloadsCount() const
    {
        return m_downloadManager.getRemainingDownloadsCount();
    }

    size_t MainWindowController::getDownloadingCount() const
    {
        return m_downloadManager.getDownloadingCount();
    }

    size_t MainWindowController::getQueuedCount() const
    {
        return m_downloadManager.getQueuedCount();
    }

    size_t MainWindowController::getCompletedCount() const
    {
        return m_downloadManager.getCompletedCount();
    }

    void MainWindowController::recoverDownloads(bool clearInstead)
    {
        if(clearInstead)
        {
            m_downloadManager.clearRecoverableDownloads();
            return;
        }
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

    void MainWindowController::clearHistory()
    {
        m_downloadManager.clearHistory();
    }

    void MainWindowController::removeHistoricDownload(const HistoricDownload& download)
    {
        m_downloadManager.removeHistoricDownload(download);
    }

    void MainWindowController::stopDownload(int id)
    {
        m_downloadManager.stopDownload(id);
    }

    void MainWindowController::pauseDownload(int id)
    {
        m_downloadManager.pauseDownload(id);
    }

    void MainWindowController::resumeDownload(int id)
    {
        m_downloadManager.resumeDownload(id);
    }

    void MainWindowController::retryDownload(int id)
    {
        m_downloadManager.retryDownload(id);
    }

    void MainWindowController::stopAllDownloads()
    {
        m_downloadManager.stopAllDownloads();
    }

    void MainWindowController::retryFailedDownloads()
    {
        m_downloadManager.retryFailedDownloads();
    }

    std::vector<int> MainWindowController::clearQueuedDownloads()
    {
        return m_downloadManager.clearQueuedDownloads();
    }

    std::vector<int> MainWindowController::clearCompletedDownloads()
    {
        return m_downloadManager.clearCompletedDownloads();
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
