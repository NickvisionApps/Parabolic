#include "controllers/mainwindowcontroller.h"
#include <future>
#include <sstream>
#include <thread>
#include <libnick/filesystem/userdirectories.h>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/documentation.h>
#include <libnick/localization/gettext.h>
#include <libnick/notifications/appnotification.h>
#include <libnick/notifications/shellnotification.h>
#include <libnick/system/environment.h>
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
#ifdef PORTABLE_BUILD
        m_configuration{ Environment::getExecutableDirectory() / "config.json" },
#else
        m_configuration{ UserDirectories::get(ApplicationUserDirectory::Config, m_appInfo.getName()) / "config.json" },
#endif
        m_keyring{ m_appInfo.getId() },
#ifdef PORTABLE_BUILD
        m_downloadManager{ Environment::getExecutableDirectory(), m_configuration },
#else
        m_downloadManager{ UserDirectories::get(ApplicationUserDirectory::Config, m_appInfo.getName()), m_configuration },
#endif
        m_isWindowActive{ false }
    {
        m_appInfo.setVersion({ "2025.11.1" });
        m_appInfo.setShortName(_("Parabolic"));
        m_appInfo.setDescription(_("Download web video and audio"));
        m_appInfo.setChangelog("- Fixed the sleep interval for multiple subtitle downloads\n- Fixed an issue where low-resolution media was being downloaded on Windows\n- Fixed an issue where aria2c couldn't download media from certain sites\n- Fixed an issue where Remove Source Data was not clearing all identifiable metadata fields");
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
        std::string translationLanguage{ m_configuration.getTranslationLanguage() };
        if(!translationLanguage.empty())
        {
            Gettext::changeLanguage(translationLanguage);
        }
        m_updater = std::make_shared<Updater>(m_appInfo.getSourceRepo());
        m_configuration.saved() += [this](const EventArgs&){ onConfigurationSaved(); };
        m_downloadManager.downloadCompleted() += [this](const DownloadCompletedEventArgs& args) { onDownloadCompleted(args); };
    }

    const AppInfo& MainWindowController::getAppInfo() const
    {
        return m_appInfo;
    }

    Theme MainWindowController::getTheme()
    {
        return m_configuration.getTheme();
    }

    VersionType MainWindowController::getPreferredUpdateType()
    {
        return m_configuration.getPreferredUpdateType();
    }

    void MainWindowController::setPreferredUpdateType(VersionType type)
    {
        m_configuration.setPreferredUpdateType(type);
    }

    void MainWindowController::setShowDisclaimerOnStartup(bool showDisclaimerOnStartup)
    {
        Configuration& config{ m_configuration };
        config.setShowDisclaimerOnStartup(showDisclaimerOnStartup);
        config.save();
    }

    void MainWindowController::setIsWindowActive(bool isWindowActive)
    {
        m_isWindowActive = isWindowActive;
    }

    Event<EventArgs>& MainWindowController::configurationSaved()
    {
        return m_configuration.saved();
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

    Event<ParamEventArgs<Version>>& MainWindowController::ytdlpUpdateAvailable()
    {
        return m_downloadManager.ytdlpUpdateAvailable();
    }

    Event<ParamEventArgs<double>>& MainWindowController::ytdlpUpdateProgressChanged()
    {
        return m_downloadManager.ytdlpUpdateProgressChanged();
    }

    std::string MainWindowController::getDebugInformation(const std::string& extraInformation) const
    {
        //yt-dlp
        std::future<std::string> ytdlpFuture{ std::async(std::launch::async, [this]() -> std::string
        {
            if(m_downloadManager.getYtdlpExecutablePath().empty())
            {
                return "yt-dlp not found\n";
            }
            else
            {
                std::string ytdlpVersion{ Environment::exec("\"" + m_downloadManager.getYtdlpExecutablePath().string() + "\" --version") };
                return "yt-dlp version " + ytdlpVersion;
            }
        }) };
        //aria2c
        std::future<std::string> aria2cFuture{ std::async(std::launch::async, []() -> std::string
        {
            if(Environment::findDependency("aria2c").empty())
            {
                return "aria2c not found\n";
            }
            else
            {
                std::string aria2cVersion{ Environment::exec("\"" + Environment::findDependency("aria2c").string() + "\" --version") };
                return aria2cVersion.substr(0, aria2cVersion.find('\n')) + "\n";
            }
        }) };
        //ffmpeg
        std::future<std::string> ffmpegFuture{ std::async(std::launch::async, []() -> std::string
        {
            if(Environment::findDependency("ffmpeg").empty())
            {
                return "ffmpeg not found\n";
            }
            else
            {
                std::string ffmpegVersion{ Environment::exec("\"" + Environment::findDependency("ffmpeg").string() + "\" -version") };
                return ffmpegVersion.substr(0, ffmpegVersion.find("Copyright")) + "\n";
            }
        }) };
        //deno
        std::future<std::string> denoFuture{ std::async(std::launch::async, []() -> std::string
        {
            if(Environment::findDependency("deno").empty())
            {
                return "deno not found\n";
            }
            else
            {
                std::string denoVersion{ Environment::exec("\"" + Environment::findDependency("deno").string() + "\" --version") };
                return denoVersion.substr(0, denoVersion.find("(")) + "\n";
            }
        }) };
        //Extra
        std::stringstream builder;
        builder << ytdlpFuture.get();
        builder << aria2cFuture.get();
        builder << ffmpegFuture.get();
        builder << denoFuture.get();
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
#ifdef PORTABLE_BUILD
        return std::make_shared<AddDownloadDialogController>(Environment::getExecutableDirectory(), m_downloadManager, m_keyring);
#else
        return std::make_shared<AddDownloadDialogController>(UserDirectories::get(ApplicationUserDirectory::Config, m_appInfo.getName()), m_downloadManager, m_keyring);
#endif
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
        return std::make_shared<PreferencesViewController>(m_configuration, m_downloadManager.getDownloadHistory());
    }

    const StartupInformation& MainWindowController::startup()
    {
        static StartupInformation info;
        if (m_started)
        {
            return info;
        }
        //Load configuration
        WindowGeometry geometry{ m_configuration.getWindowGeometry() };
        if(geometry.getX() < 0 || geometry.getY() < 0)
        {
            geometry.setX(10);
            geometry.setY(10);
        }
        info.setWindowGeometry(geometry);
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
        info.setShowDisclaimer(m_configuration.getShowDisclaimerOnStartup());
        //Get URL to validate from args
        if(m_args.size() > 1)
        {
            std::string url{ StringHelpers::trim(m_args[1]) };
            if(url.starts_with("parabolic://"))
            {
                url = url.substr(12);
            }
            if(StringHelpers::isValidUrl(url))
            {
                info.setUrlToValidate(url);
            }
        }
        m_started = true;
        return info;
    }

    void MainWindowController::shutdown(const WindowGeometry& geometry)
    {
        //Save config
        Configuration& config{ m_configuration };
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
                double progress{ static_cast<double>(static_cast<long double>(downloadNow) / static_cast<long double>(downloadTotal)) };
                if(progress != 1.0)
                {
                    m_appUpdateProgressChanged.invoke({ progress });
                }
                return true;
            } }) };
            m_appUpdateProgressChanged.invoke({ 1.0 });
            if(!res)
            {
                AppNotification::send({ _("Unable to download and install update"), NotificationSeverity::Error });
            }
        } };
        worker.detach();
    }
#endif

    void MainWindowController::startYtdlpUpdate()
    {
        m_downloadManager.startYtdlpUpdate();
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
        if(m_configuration.getPreventSuspend())
        {
            m_suspendInhibitor.inhibit();
        }
        else
        {
            m_suspendInhibitor.uninhibit();
        }
        m_downloadManager.setDownloaderOptions(m_configuration.getDownloaderOptions());
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
            ShellNotification::send({ _("Download Failed"), _f("{} has finished with an error", args.getPath().filename().string()), NotificationSeverity::Error }, m_appInfo);
        }
    }
}
