#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <vector>
#include <libnick/events/event.h>
#include <libnick/events/parameventargs.h>
#include <libnick/helpers/cancellationtoken.h>
#include <libnick/keyring/credential.h>
#include "configuration.h"
#include "download.h"
#include "downloaderoptions.h"
#include "downloadhistory.h"
#include "downloadrecoveryqueue.h"
#include "historicdownload.h"
#include "startupinformation.h"
#include "urlinfo.h"
#include "ytdlpmanager.h"
#include "events/downloadaddedeventargs.h"
#include "events/downloadcompletedeventargs.h"
#include "events/downloadcredentialneededeventargs.h"
#include "events/downloadprogresschangedeventargs.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief A manager of downloads.
     */
    class DownloadManager
    {
    public:
        /**
         * @brief Constructs a DownloadManager.
         * @param dataDirPath The path to save data files for the application to
         * @param configuration The Configuration
         */
        DownloadManager(const std::filesystem::path& dataDirPath, Configuration& configuration);
        /**
         * @brief Destructs a DownloadManager.
         */
        ~DownloadManager();
        /**
         * @brief Gets the event for when an yt-dlp update is available.
         * @return The yt-dlp update available event
         */
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<Nickvision::Update::Version>>& ytdlpUpdateAvailable();
        /**
         * @brief Gets the event for when an yt-dlp update's progress is changed.
         * @return The yt-dlp update progress changed event
         */
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<double>>& ytdlpUpdateProgressChanged();
        /**
         * @brief Gets the event for when the history is changed.
         * @return The history changed event
         */
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<std::vector<Models::HistoricDownload>>>& historyChanged();
        /**
         * @brief Gets the event for when a download is added.
         * @return The download added event
         */
        Nickvision::Events::Event<Events::DownloadAddedEventArgs>& downloadAdded();
        /**
         * @brief Gets the event for when a download is completed.
         * @return The download completed event
         */
        Nickvision::Events::Event<Events::DownloadCompletedEventArgs>& downloadCompleted();
        /**
         * @brief Gets the event for when a download's progress is changed.
         * @return The download progress changed event
         */
        Nickvision::Events::Event<Events::DownloadProgressChangedEventArgs>& downloadProgressChanged();
        /**
         * @brief Gets the event for when a download is stopped.
         * @return The download stopped event
         */
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<int>>& downloadStopped();
        /**
         * @brief Gets the event for when a download is paused.
         * @return The download paused event
         */
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<int>>& downloadPaused();
        /**
         * @brief Gets the event for when a download is resumed.
         * @return The download resumed event
         */
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<int>>& downloadResumed();
        /**
         * @brief Gets the event for when a download is retried.
         * @return The download retried event
         */
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<int>>& downloadRetried();
        /**
         * @brief Gets the event for when a download is started from the queue.
         * @return The download started from queue event
         */
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<int>>& downloadStartedFromQueue();
        /**
         * @brief Gets the event for when a credential is needed for a download.
         * @return The download credential needed event
         */
        Nickvision::Events::Event<Events::DownloadCredentialNeededEventArgs>& downloadCredentialNeeded();
        /**
         * @brief Gets the path to the yt-dlp executable.
         * @return The path to the yt-dlp executable
         */
        const std::filesystem::path& getYtdlpExecutablePath() const;
        /**
         * @brief Gets the options used for the downloader.
         * @return The DownloaderOptions
         */
        const DownloaderOptions& getDownloaderOptions() const;
        /**
         * @brief Sets the options to use for the downloader.
         * @param options The DownloaderOptions
         */
        void setDownloaderOptions(const DownloaderOptions& options);
        /**
         * @brief Gets the count of remaining downloads.
         * @return The count of remaining downloads
         */
        size_t getRemainingDownloadsCount() const;
        /**
         * @brief Gets the count of downloading downloads.
         * @return The count of downloading downloads
         */
        size_t getDownloadingCount() const;
        /**
         * @brief Gets the count of queued downloads.
         * @return The count of queued downloads
         */
        size_t getQueuedCount() const;
        /**
         * @brief Gets the count of completed downloads.
         * @return The count of completed downloads
         */
        size_t getCompletedCount() const;
        /**
         * @brief Gets the download history.
         * @return The download history
         */
        DownloadHistory& getDownloadHistory();
        /**
         * @brief Loads the DownloadManager.
         * @brief This method invokes the historyChanged event.
         * @param info The StartupInformation object to edit
         */
        void startup(StartupInformation& info);
        /**
         * @brief Starts downloading the latest yt-dlp update in the background.
         */
        void startYtdlpUpdate();
        /**
         * @brief Recovers all available recoverable downloads.
         * @return The number of downloads recovered
         */
        size_t recoverDownloads();
        /**
         * @brief Clears all available recoverable downloads.
         */
        void clearRecoverableDownloads();
        /**
         * @brief Clears the download history.
         * @brief This method invokes the historyChanged event.
         */
        void clearHistory();
        /**
         * @brief Removes a historic download from the history.
         * @brief This method invokes the historyChanged event.
         * @param download The historic download to remove
         */
        void removeHistoricDownload(const HistoricDownload& download);
        /**
         * @brief Fetches information about a URL.
         * @param token The CancellationToken to use for cancellation
         * @param url The URL to fetch information for
         * @param credential An optional credential to use for authentication
         * @param suggestedSaveFolder An option save folder to save the url's media to
         * @return The UrlInfo if successful, else std::nullopt
         */
        std::optional<UrlInfo> fetchUrlInfo(Helpers::CancellationToken& token, const std::string& url, const std::optional<Keyring::Credential>& credential, const std::filesystem::path& suggestedSaveFolder = {}) const;
        /**
         * @brief Fetches information about a set of URLs from a batch file.
         * @param token The CancellationToken to use for cancellation
         * @param batchFile The batch file with listed URLs
         * @param credential An optional credential to use for authentication
         * @return The UrlInfo if successful, else std::nullopt
         */
        std::optional<UrlInfo> fetchUrlInfo(Helpers::CancellationToken& token, const std::filesystem::path& batchFile, const std::optional<Keyring::Credential>& credential) const;
        /**
         * @brief Adds a download to the queue.
         * @brief This will invoke the downloadAdded event if added successfully.
         * @param options The options for the download
         * @param excludeFromHistory Whether or not to exclude the download from the history
         */
        void addDownload(const DownloadOptions& options, bool excludeFromHistory);
        /**
         * @brief Requests that a download be stopped.
         * @brief This will invoke the downloadStopped event if stopped successfully.
         * @param id The id of the download to stop
         */
        void stopDownload(int id);
        /**
         * @brief Requests that a download be paused.
         * @brief This will invoke the downloadPaused event if stopped successfully.
         * @param id The id of the download to pause
         */
        void pauseDownload(int id);
        /**
         * @brief Requests that a download be resumed.
         * @brief This will invoke the downloadResumed event if stopped successfully.
         * @param id The id of the download to resume
         */
        void resumeDownload(int id);
        /**
         * @brief Requests that a download be retried.
         * @brief This will invoke the downloadRetried event if retried successfully.
         * @param id The id of the download to retry
         */
        void retryDownload(int id);
        /**
         * @brief Requests that all downloads be stopped.
         * @brief This will invoke the downloadStopped event for each download stopped.
         */
        void stopAllDownloads();
        /**
         * @brief Requests that all failed downloads be retried.
         * @brief This will invoke the downloadRetried event for each download retried.
         */
        void retryFailedDownloads();
        /**
         * @brief Clears all downloads from the queue.
         * @return The ids of the downloads cleared
         */
        std::vector<int> clearQueuedDownloads();
        /**
         * @brief Clears all completed downloads.
         * @return The ids of the downloads cleared
         */
        std::vector<int> clearCompletedDownloads();

    private:
        /**
         * @brief Adds a download to the queue.
         * @param download The download to add
         * @param excludeFromHistory Whether or not to exclude the download from the history
         */
        void addDownload(const std::shared_ptr<Download>& download, bool excludeFromHistory);
        /**
         * @brief Handles when a download's progress is changed.
         * @param args Events::DownloadProgressChangedEventArgs
         */
        void onDownloadProgressChanged(const Events::DownloadProgressChangedEventArgs& args);
        /**
         * @brief Handles when a download is completed.
         * @param args Events::DownloadCompletedEventArgs
         */
        void onDownloadCompleted(const Events::DownloadCompletedEventArgs& args);
        mutable std::mutex m_mutex;
        YtdlpManager m_ytdlpManager;
        DownloaderOptions m_options;
        DownloadHistory m_history;
        DownloadRecoveryQueue m_recoveryQueue;
        std::unordered_map<int, std::shared_ptr<Download>> m_downloading;
        std::unordered_map<int, std::shared_ptr<Download>> m_queued;
        std::unordered_map<int, std::shared_ptr<Download>> m_completed;
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<std::vector<HistoricDownload>>> m_historyChanged;
        Nickvision::Events::Event<Events::DownloadAddedEventArgs> m_downloadAdded;
        Nickvision::Events::Event<Events::DownloadCompletedEventArgs> m_downloadCompleted;
        Nickvision::Events::Event<Events::DownloadProgressChangedEventArgs> m_downloadProgressChanged;
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<int>> m_downloadStopped;
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<int>> m_downloadPaused;
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<int>> m_downloadResumed;
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<int>> m_downloadRetried;
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<int>> m_downloadStartedFromQueue;
        Nickvision::Events::Event<Events::DownloadCredentialNeededEventArgs> m_downloadCredentialNeeded;
    };
}

#endif //DOWNLOADMANAGER_H
