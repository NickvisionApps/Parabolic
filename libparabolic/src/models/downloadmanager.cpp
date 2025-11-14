#include "models/downloadmanager.h"
#include <stdexcept>
#include <libnick/filesystem/userdirectories.h>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/system/environment.h>
#include <libnick/system/process.h>
#include "models/batchfile.h"

using namespace Nickvision::Events;
using namespace Nickvision::Filesystem;
using namespace Nickvision::Helpers;
using namespace Nickvision::Keyring;
using namespace Nickvision::System;
using namespace Nickvision::TubeConverter::Shared::Events;
using namespace Nickvision::Update;

namespace Nickvision::TubeConverter::Shared::Models
{
    static std::string s_empty{};

    DownloadManager::DownloadManager(const std::filesystem::path& dataDirPath, Configuration& configuration)
        : m_ytdlpManager{ configuration },
        m_options{ configuration.getDownloaderOptions() },
        m_history{ dataDirPath / "history.json" },
        m_recoveryQueue{ dataDirPath / "recovery.json" }
    {
        m_history.saved() += [this](const EventArgs&){ m_historyChanged.invoke(m_history.getHistory()); };
    }

    DownloadManager::~DownloadManager()
    {
        stopAllDownloads();
    }

    Event<ParamEventArgs<Version>>& DownloadManager::ytdlpUpdateAvailable()
    {
        return m_ytdlpManager.updateAvailable();
    }

    Event<ParamEventArgs<double>>& DownloadManager::ytdlpUpdateProgressChanged()
    {
        return m_ytdlpManager.updateProgressChanged();
    }

    Event<ParamEventArgs<std::vector<HistoricDownload>>>& DownloadManager::historyChanged()
    {
        return m_historyChanged;
    }

    Event<DownloadAddedEventArgs>& DownloadManager::downloadAdded()
    {
        return m_downloadAdded;
    }

    Event<DownloadCompletedEventArgs>& DownloadManager::downloadCompleted()
    {
        return m_downloadCompleted;
    }

    Event<DownloadProgressChangedEventArgs>& DownloadManager::downloadProgressChanged()
    {
        return m_downloadProgressChanged;
    }

    Event<ParamEventArgs<int>>& DownloadManager::downloadStopped()
    {
        return m_downloadStopped;
    }

    Event<ParamEventArgs<int>>& DownloadManager::downloadPaused()
    {
        return m_downloadPaused;
    }

    Event<ParamEventArgs<int>>& DownloadManager::downloadResumed()
    {
        return m_downloadResumed;
    }

    Event<ParamEventArgs<int>>& DownloadManager::downloadRetried()
    {
        return m_downloadRetried;
    }

    Event<ParamEventArgs<int>>& DownloadManager::downloadStartedFromQueue()
    {
        return m_downloadStartedFromQueue;
    }

    Event<DownloadCredentialNeededEventArgs>& DownloadManager::downloadCredentialNeeded()
    {
        return m_downloadCredentialNeeded;
    }

    const std::filesystem::path& DownloadManager::getYtdlpExecutablePath() const
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        return m_ytdlpManager.getExecutablePath();
    }

    const DownloaderOptions& DownloadManager::getDownloaderOptions() const
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        return m_options;
    }

    void DownloadManager::setDownloaderOptions(const DownloaderOptions& options)
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        m_options = options;
        lock.unlock();
        while(m_downloading.size() < static_cast<size_t>(m_options.getMaxNumberOfActiveDownloads()) && !m_queued.empty())
        {
            lock.lock();
            std::shared_ptr<Download> firstQueuedDownload{ (*m_queued.begin()).second };
            m_downloading.emplace(firstQueuedDownload->getId(), firstQueuedDownload);
            m_queued.erase(firstQueuedDownload->getId());
            lock.unlock();
            m_downloadStartedFromQueue.invoke(firstQueuedDownload->getId());
            firstQueuedDownload->start(m_ytdlpManager.getExecutablePath(), m_options);
        }
    }

    size_t DownloadManager::getRemainingDownloadsCount() const
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        return m_downloading.size() + m_queued.size();
    }

    size_t DownloadManager::getDownloadingCount() const
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        return m_downloading.size();
    }

    size_t DownloadManager::getQueuedCount() const
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        return m_queued.size();
    }

    size_t DownloadManager::getCompletedCount() const
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        return m_completed.size();
    }

    DownloadHistory& DownloadManager::getDownloadHistory()
    {
        return m_history;
    }

    void DownloadManager::startup(StartupInformation& info)
    {
        m_ytdlpManager.checkForUpdates();
        m_historyChanged.invoke(m_history.getHistory());
        info.setHasRecoverableDownloads(m_recoveryQueue.getRecoverableDownloads().size() > 0);
    }

    void DownloadManager::startYtdlpUpdate()
    {
        m_ytdlpManager.startUpdateDownload();
    }

    size_t DownloadManager::recoverDownloads()
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        std::unordered_map<int, std::pair<DownloadOptions, bool>> queue{ m_recoveryQueue.getRecoverableDownloads() };
        m_recoveryQueue.clear();
        lock.unlock();
        for(std::pair<const int, std::pair<DownloadOptions, bool>>& pair : queue)
        {
            if(pair.second.second)
            {
                std::shared_ptr<Credential> credential{ std::make_shared<Credential>("", "", "", "") };
                while(credential->getUsername().empty() && credential->getPassword().empty())
                {
                    m_downloadCredentialNeeded.invoke({ pair.second.first.getUrl(), credential });
                }
                pair.second.first.setCredential(*credential);
            }
            addDownload(pair.second.first, false);
        }
        return queue.size();
    }

    void DownloadManager::clearRecoverableDownloads()
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        m_recoveryQueue.clear();
    }

    void DownloadManager::clearHistory()
    {
        m_history.clear();
    }

    void DownloadManager::removeHistoricDownload(const HistoricDownload& download)
    {
        m_history.removeDownload(download);
    }

    std::optional<UrlInfo> DownloadManager::fetchUrlInfo(CancellationToken& token, const std::string& url, const std::optional<Credential>& credential, const std::filesystem::path& suggestedSaveFolder, const std::string& suggestedFilename) const
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        std::vector<std::string> arguments{ "--ignore-config", "--xff", "default", "--dump-single-json", "--skip-download", "--ignore-errors", "--no-warnings" };
        arguments.push_back("--ffmpeg-location");
        arguments.push_back(Environment::findDependency("ffmpeg").string());
        arguments.push_back("--js-runtimes");
        arguments.push_back("deno:" + Environment::findDependency("deno").string());
        arguments.push_back("--plugin-dir");
        arguments.push_back((Environment::getExecutableDirectory() / "plugins").string());
        if(url.find("soundcloud.com") == std::string::npos)
        {
            arguments.push_back("--flat-playlist");
        }
        if(m_options.getLimitCharacters())
        {
            arguments.push_back("--windows-filenames");
        }
        if(!m_options.getProxyUrl().empty())
        {
            arguments.push_back("--proxy");
            arguments.push_back(m_options.getProxyUrl());
        }
        if(credential)
        {
            arguments.push_back("--username");
            arguments.push_back(credential->getUsername());
            arguments.push_back("--password");
            arguments.push_back(credential->getPassword());
        }
        if(m_options.getCookiesBrowser() != Browser::None)
        {
            arguments.push_back("--cookies-from-browser");
            switch(m_options.getCookiesBrowser())
            {
            case Browser::Brave:
                arguments.push_back("brave");
                break;
            case Browser::Chrome:
                arguments.push_back("chrome");
                break;
            case Browser::Chromium:
                arguments.push_back("chromium");
                break;
            case Browser::Edge:
                arguments.push_back("edge");
                break;
            case Browser::Firefox:
                arguments.push_back("firefox");
                break;
            case Browser::Opera:
                arguments.push_back("opera");
                break;
            case Browser::Vivaldi:
                arguments.push_back("vivaldi");
                break;
            case Browser::Whale:
                arguments.push_back("whale");
                break;
            default:
                break;
            }
        }
        else if(std::filesystem::exists(m_options.getCookiesPath()))
        {
            arguments.push_back("--cookies");
            arguments.push_back(m_options.getCookiesPath().string());
        }
        arguments.push_back("--paths");
        arguments.push_back("temp:" + UserDirectories::get(UserDirectory::Cache).string());
        arguments.push_back(url);
        if(token)
        {
            return std::nullopt;
        }
        Process process{ m_ytdlpManager.getExecutablePath(), arguments };
        lock.unlock();
        token.setCancelFunction([&process]()
        {
            process.kill();
        });
        process.start();
        if(process.waitForExit() != 0 || process.getOutput().empty())
        {
            token.setCancelFunction({});
            throw std::runtime_error(process.getOutput());
        }
        token.setCancelFunction({});
        boost::json::value info = boost::json::parse(process.getOutput()[0] == '{' ? process.getOutput() : process.getOutput().substr(process.getOutput().find('{')));
        if(!info.is_object())
        {
            throw std::runtime_error(process.getOutput());
        }
        boost::json::object obj = info.as_object();
        lock.lock();
        obj["limit_characters"] = m_options.getLimitCharacters();
        obj["include_media_id_in_title"] = m_options.getIncludeMediaIdInTitle();
        obj["include_auto_generated_subtitles"] = m_options.getIncludeAutoGeneratedSubtitles();
        obj["preferred_video_codec"] = static_cast<int>(m_options.getPreferredVideoCodec());
        obj["preferred_audio_codec"] = static_cast<int>(m_options.getPreferredAudioCodec());
        lock.unlock();
        if(!suggestedSaveFolder.empty())
        {
            obj["suggested_save_folder"] = suggestedSaveFolder.string();
        }
        //Handle YouTube Tabs as they require individual fetching
        if(obj.contains("entries") && obj["entries"].is_array())
        {
            std::vector<std::future<std::optional<UrlInfo>>> urlInfoFutures;
            for(const boost::json::value& entry : obj["entries"].as_array())
            {
                if(token)
                {
                    return std::nullopt;
                }
                if(entry.is_object())
                {
                    boost::json::object e = entry.as_object();
                    //Check for YouTube Tab
                    if(e.contains("ie_key") && e["ie_key"].is_string() && e["ie_key"].as_string() == "YoutubeTab")
                    {
                        urlInfoFutures.push_back(fetchUrlInfoAsync(token, std::string(e["url"].as_string().c_str()), credential, suggestedSaveFolder));
                    }
                }
            }
            std::vector<UrlInfo> urlInfos;
            for(std::future<std::optional<UrlInfo>>& future : urlInfoFutures)
            {
                if(token)
                {
                    return std::nullopt;
                }
                std::optional<UrlInfo> urlInfo{ future.get() };
                if(urlInfo)
                {
                    urlInfos.push_back(*urlInfo);
                }
            }
            if(token)
            {
                return std::nullopt;
            }
            if(!urlInfos.empty())
            {
                UrlInfo urlInfo{ url, obj["title"].is_string() ? obj["title"].as_string().c_str() : "Tab", urlInfos };
                if(urlInfo.count() <= 0)
                {
                    throw std::logic_error("No media found");
                }
                return urlInfo;
            }
        }
        else if(!suggestedFilename.empty()) //Set suggested filename if provided for single media only
        {
            obj["title"] = suggestedFilename;
        }
        if(token)
        {
            return std::nullopt;
        }
        UrlInfo urlInfo{ url, obj };
        if(urlInfo.count() <= 0)
        {
            throw std::logic_error("No media found");
        }
        return urlInfo;
    }

    std::future<std::optional<UrlInfo>> DownloadManager::fetchUrlInfoAsync(CancellationToken& token, const std::string& url, const std::optional<Credential>& credential, const std::filesystem::path& suggestedSaveFolder, const std::string& suggestedFilename) const
    {
        return std::async(std::launch::async, [this, &token, url, credential, suggestedSaveFolder, suggestedFilename]() -> std::optional<UrlInfo>
        {
            return fetchUrlInfo(token, url, credential, suggestedSaveFolder, suggestedFilename);
        });
    }

    std::optional<UrlInfo> DownloadManager::fetchUrlInfo(CancellationToken& token, const std::filesystem::path& batchFilePath, const std::optional<Credential>& credential) const
    {
        //Parse batch file
        BatchFile batchFile{ batchFilePath };
        if(batchFile.getEntries().empty())
        {
            return std::nullopt;
        }
        //Start fetching UrlInfo objects for each URL in the batch file
        std::vector<std::future<std::optional<UrlInfo>>> urlInfoFutures;
        for(const BatchFileEntry& entry : batchFile.getEntries())
        {
            if(token)
            {
                return std::nullopt;
            }
            urlInfoFutures.push_back(fetchUrlInfoAsync(token, entry.getUrl(), credential, entry.getSuggestedSaveFolder(), entry.getSuggestedFilename()));
        }
        //Collect UrlInfo objects
        std::vector<UrlInfo> urlInfos;
        for(std::future<std::optional<UrlInfo>>& future : urlInfoFutures)
        {
            if(token)
            {
                return std::nullopt;
            }
            std::optional<UrlInfo> urlInfo{ future.get() };
            if(urlInfo)
            {
                urlInfos.push_back(*urlInfo);
            }
        }
        //Build final UrlInfo
        if(token || urlInfos.empty())
        {
            return std::nullopt;
        }
        return UrlInfo{ batchFile.getPath().string(), batchFile.getPath().filename().stem().string(), urlInfos };
    }

    void DownloadManager::addDownload(const DownloadOptions& options, bool excludeFromHistory)
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        //Build a Download object
        std::shared_ptr<Download> download{ std::make_shared<Download>(options) };
        download->progressChanged() += [this](const DownloadProgressChangedEventArgs& args){ onDownloadProgressChanged(args); };
        download->completed() += [this](const DownloadCompletedEventArgs& args){ onDownloadCompleted(args); };
        lock.unlock();
        //Add the download
        addDownload(download, excludeFromHistory);
    }

    void DownloadManager::stopDownload(int id)
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        bool stopped{ false };
        if(m_downloading.contains(id))
        {
            m_downloading.at(id)->stop();
            m_completed.emplace(id, m_downloading.at(id));
            m_downloading.erase(id);
            stopped = true;
        }
        else if(m_queued.contains(id))
        {
            m_completed.emplace(id, m_queued.at(id));
            m_queued.erase(id);
            stopped = true;
        }
        if(stopped)
        {
            m_recoveryQueue.remove(id);
            lock.unlock();
            m_downloadStopped.invoke(id);
        }
    }

    void DownloadManager::pauseDownload(int id)
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        if(m_downloading.contains(id))
        {
            m_downloading.at(id)->pause();
            m_downloadPaused.invoke(id);
        }
    }

    void DownloadManager::resumeDownload(int id)
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        if(m_downloading.contains(id))
        {
            m_downloading.at(id)->resume();
            m_downloadResumed.invoke(id);
        }
    }

    void DownloadManager::retryDownload(int id)
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        if(m_completed.contains(id))
        {
            std::shared_ptr<Download> download{ m_completed.at(id) };
            m_completed.erase(id);
            lock.unlock();
            m_downloadRetried.invoke(id);
            addDownload(download, true);
        }
    }

    void DownloadManager::stopAllDownloads()
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        //Get Downloading and Queued keys
        std::vector<int> keys;
        keys.reserve(m_downloading.size() + m_queued.size());
        for(const std::pair<const int, std::shared_ptr<Download>>& pair : m_downloading)
        {
            keys.push_back(pair.first);
        }
        for(const std::pair<const int, std::shared_ptr<Download>>& pair : m_queued)
        {
            keys.push_back(pair.first);
        }
        lock.unlock();
        //Stop downloads
        for(int key : keys)
        {
            stopDownload(key);
        }
    }

    void DownloadManager::retryFailedDownloads()
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        //Get Completed keys
        std::vector<int> keys;
        for(const std::pair<const int, std::shared_ptr<Download>>& pair : m_completed)
        {
            if(pair.second->getStatus() == DownloadStatus::Error)
            {
                keys.push_back(pair.first);
            }
        }
        lock.unlock();
        //Retry downloads
        for(int key : keys)
        {
            retryDownload(key);
        }
    }

    std::vector<int> DownloadManager::clearQueuedDownloads()
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        std::vector<int> cleared;
        for(const std::pair<const int, std::shared_ptr<Download>>& pair : m_queued)
        {
            cleared.push_back(pair.first);
        }
        m_queued.clear();
        return cleared;
    }

    std::vector<int> DownloadManager::clearCompletedDownloads()
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        std::vector<int> cleared;
        for(const std::pair<const int, std::shared_ptr<Download>>& pair : m_completed)
        {
            cleared.push_back(pair.first);
        }
        m_completed.clear();
        return cleared;
    }

    void DownloadManager::addDownload(const std::shared_ptr<Download>& download, bool excludeFromHistory)
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        m_recoveryQueue.add(download->getId(), download->getOptions());
        if(!excludeFromHistory)
        {
            m_history.addDownload({ download->getUrl(), download->getPath().filename().stem().string(), download->getPath() });
        }
        if(m_downloading.size() < static_cast<size_t>(m_options.getMaxNumberOfActiveDownloads()))
        {
            m_downloading.emplace(download->getId(), download);
            lock.unlock();
            m_downloadAdded.invoke({ download->getId(), download->getPath(), download->getUrl(), DownloadStatus::Running });
            download->start(m_ytdlpManager.getExecutablePath(), m_options);
        }
        else
        {
            m_queued.emplace(download->getId(), download);
            lock.unlock();
            m_downloadAdded.invoke({ download->getId(), download->getPath(), download->getUrl(), download->getStatus() });
        }
    }

    void DownloadManager::onDownloadProgressChanged(const DownloadProgressChangedEventArgs& args)
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        if(!m_downloading.contains(args.getId()))
        {
            return;
        }
        const std::shared_ptr<Download>& download{ m_downloading.at(args.getId()) };
        if(download->getStatus() == DownloadStatus::Stopped)
        {
            return;
        }
        lock.unlock();
        m_downloadProgressChanged.invoke(args);
    }

    void DownloadManager::onDownloadCompleted(const DownloadCompletedEventArgs& args)
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        if(!m_downloading.contains(args.getId()))
        {
            return;
        }
        std::shared_ptr<Download> download{ m_downloading.at(args.getId()) };
        if(download->getStatus() == DownloadStatus::Stopped)
        {
            return;
        }
        m_completed.emplace(download->getId(), download);
        m_downloading.erase(download->getId());
        m_recoveryQueue.remove(download->getId());
        lock.unlock();
        m_downloadCompleted.invoke(args);
        lock.lock();
        //Start Download from Queue if There is Space
        if(m_downloading.size() < static_cast<size_t>(m_options.getMaxNumberOfActiveDownloads()) && !m_queued.empty())
        {
            std::shared_ptr<Download> firstQueuedDownload{ (*m_queued.begin()).second };
            m_downloading.emplace(firstQueuedDownload->getId(), firstQueuedDownload);
            m_queued.erase(firstQueuedDownload->getId());
            lock.unlock();
            m_downloadStartedFromQueue.invoke(firstQueuedDownload->getId());
            firstQueuedDownload->start(m_ytdlpManager.getExecutablePath(), m_options);
        }
    }
}
