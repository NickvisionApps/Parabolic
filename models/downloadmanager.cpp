#include "downloadmanager.h"

using namespace NickvisionTubeConverter::Models;

DownloadManager::DownloadManager() : m_maxNumOfDownloads{5}, m_log{""}, m_successfulDownloads{0}
{

}

unsigned int DownloadManager::getMaxNumOfDownloads() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_maxNumOfDownloads;
}

void DownloadManager::setMaxNumOfDownloads(unsigned int maxNumOfDownloads)
{
    std::lock_guard<std::mutex> lock{m_mutex};
    m_maxNumOfDownloads = maxNumOfDownloads;
}

const std::vector<std::shared_ptr<Download>>& DownloadManager::getDownloads() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_downloads;
}

const std::string& DownloadManager::getLog() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_log;
}

unsigned int DownloadManager::getSuccessfulDownloads() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_successfulDownloads;
}

bool DownloadManager::isQueueFull() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_downloads.size() == m_maxNumOfDownloads;
}

size_t DownloadManager::getQueueCount() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_downloads.size();
}

bool DownloadManager::addToQueue(const std::shared_ptr<Download>& download)
{
    std::lock_guard<std::mutex> lock{m_mutex};
    if(m_downloads.size() == m_maxNumOfDownloads)
    {
        return false;
    }
    m_downloads.push_back(download);
    return true;
}

bool DownloadManager::removeFromQueue(std::size_t index)
{
    std::lock_guard<std::mutex> lock{m_mutex};
    if(m_downloads.size() == 0 || index > m_downloads.size() - 1)
    {
        return false;
    }
    m_downloads.erase(m_downloads.begin() + index);
    return true;
}

void DownloadManager::clearQueue()
{
    std::lock_guard<std::mutex> lock{m_mutex};
    m_downloads.clear();
}

unsigned int DownloadManager::downloadAll()
{
    std::lock_guard<std::mutex> lock{m_mutex};
    m_log = "";
    m_successfulDownloads = 0;
    for(const std::shared_ptr<Download>& download : m_downloads)
    {
        std::pair<bool, std::string> result{download->download()};
        m_log += result.second;
        if(result.first)
        {
            m_successfulDownloads++;
        }
    }
    m_downloads.clear();
    return m_successfulDownloads;
}
