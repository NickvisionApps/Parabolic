#include "models/downloadmanager.h"

using namespace Nickvision::Events;
using namespace Nickvision::Logging;
using namespace Nickvision::TubeConverter::Shared::Events;

namespace Nickvision::TubeConverter::Shared::Models
{
    DownloadManager::DownloadManager(const DownloaderOptions& options, DownloadHistory& history, Logger& logger)
        : m_options{ options },
        m_history{ history },
        m_logger{ logger }
    {
        m_history.saved() += [this](const EventArgs&){ m_historyChanged.invoke(m_history.getHistory()); };
    }

    DownloadManager::~DownloadManager()
    {
        stopAllDownloads();
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

    Event<ParamEventArgs<int>>& DownloadManager::downloadRetried()
    {
        return m_downloadRetried;
    }

    Event<ParamEventArgs<int>>& DownloadManager::downloadStartedFromQueue()
    {
        return m_downloadStartedFromQueue;
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
            firstQueuedDownload->start(m_options);
        }
    }

    const std::string& DownloadManager::getDownloadLog(int id) const
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        if(m_downloading.contains(id))
        {
            return m_downloading.at(id)->getLog();
        }
        if(m_queued.contains(id))
        {
            return m_queued.at(id)->getLog();
        }
        if(m_completed.contains(id))
        {
            return m_completed.at(id)->getLog();
        }
        static std::string empty;
        return empty;
    }

    DownloadStatus DownloadManager::getDownloadStatus(int id) const
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        if(m_downloading.contains(id))
        {
            return m_downloading.at(id)->getStatus();
        }
        if(m_queued.contains(id))
        {
            return m_queued.at(id)->getStatus();
        }
        if(m_completed.contains(id))
        {
            return m_completed.at(id)->getStatus();
        }
        return DownloadStatus::Queued;
    }

    void DownloadManager::loadHistory()
    {
        m_logger.log(LogLevel::Info, "Loaded " + std::to_string(m_history.getHistory().size()) + " historic downloads.");
        m_historyChanged.invoke(m_history.getHistory());
    }

    void DownloadManager::clearHistory()
    {
        if(m_history.clear())
        {
            m_logger.log(LogLevel::Info, "Cleared download history.");
        }
    }

    void DownloadManager::removeHistoricDownload(const HistoricDownload& download)
    {
        if(m_history.removeDownload(download))
        {
            m_logger.log(LogLevel::Info, "Removed historic download: " + download.getTitle());
        }
    }

    void DownloadManager::addDownload(const DownloadOptions& options)
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        std::shared_ptr<Download> download{ std::make_shared<Download>(options) };
        download->progressChanged() += [this](const DownloadProgressChangedEventArgs& args){ onDownloadProgressChanged(args); };
        download->completed() += [this](const DownloadCompletedEventArgs& args){ onDownloadCompleted(args); };
        lock.unlock();
        addDownload(download);
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
        lock.unlock();
        if(stopped)
        {
            m_logger.log(LogLevel::Info, "Stopped download (" + std::to_string(id) + ").");
            m_downloadStopped.invoke(id);
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
            addDownload(download);
            m_logger.log(LogLevel::Info, "Retried download (" + std::to_string(id) + ").");
        }
    }

    void DownloadManager::stopAllDownloads()
    {
        for(const std::pair<const int, std::shared_ptr<Download>>& pair : m_downloading)
        {
            stopDownload(pair.first);
        }
        for(const std::pair<const int, std::shared_ptr<Download>>& pair : m_queued)
        {
            stopDownload(pair.first);
        }
    }

    void DownloadManager::retryFailedDownloads()
    {
        for(const std::pair<const int, std::shared_ptr<Download>>& pair : m_completed)
        {
            if(pair.second->getStatus() == DownloadStatus::Error)
            {
                retryDownload(pair.first);
            }
        }
    }

    std::vector<int> DownloadManager::clearQueuedDownloads()
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        std::vector<int> cleared;
        for(const std::pair<const int, std::shared_ptr<Download>>& pair : m_queued)
        {
            cleared.push_back(pair.first);
            m_logger.log(LogLevel::Info, "Cleared download (" + std::to_string(pair.first) + ") from queue.");
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
            m_logger.log(LogLevel::Info, "Cleared completed download (" + std::to_string(pair.first) + ").");
        }
        m_completed.clear();
        return cleared;
    }

    void DownloadManager::addDownload(const std::shared_ptr<Download>& download)
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        if(m_downloading.size() < static_cast<size_t>(m_options.getMaxNumberOfActiveDownloads()))
        {
            m_downloading.emplace(download->getId(), download);
            lock.unlock();
            m_downloadAdded.invoke({ download->getId(), download->getPath(), download->getUrl(), DownloadStatus::Running });
            download->start(m_options);
        }
        else
        {
            m_queued.emplace(download->getId(), download);
            lock.unlock();
            m_downloadAdded.invoke({ download->getId(), download->getPath(), download->getUrl(), download->getStatus() });
        }
        m_logger.log(LogLevel::Info, "Added download (" + std::to_string(download->getId()) + ") " + download->getUrl());
        m_history.addDownload({ download->getUrl(), download->getPath().filename().stem().string(), download->getPath() });
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
        m_logger.log(LogLevel::Info, "Download progress changed (" + std::to_string(args.getId()) + ").");
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
        m_logger.log(LogLevel::Info, "Download completed (" + std::to_string(args.getId()) + ").");
        m_completed.emplace(download->getId(), download);
        m_downloading.erase(download->getId());
        lock.unlock();
        m_downloadCompleted.invoke(args);
        lock.lock();
        if(m_downloading.size() < static_cast<size_t>(m_options.getMaxNumberOfActiveDownloads()) && !m_queued.empty())
        {
            std::shared_ptr<Download> firstQueuedDownload{ (*m_queued.begin()).second };
            m_downloading.emplace(firstQueuedDownload->getId(), firstQueuedDownload);
            m_queued.erase(firstQueuedDownload->getId());
            lock.unlock();
            m_downloadStartedFromQueue.invoke(firstQueuedDownload->getId());
            firstQueuedDownload->start(m_options);
        }
    }
}