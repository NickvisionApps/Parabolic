#include "downloadmanager.h"

namespace NickvisionTubeConverter::Models
{
    DownloadManager::DownloadManager() : m_maxNumOfDownloads(5)
    {

    }

    int DownloadManager::getMaxNumOfDownloads() const
    {
        return m_maxNumOfDownloads;
    }

    void DownloadManager::setMaxNumOfDownloads(int maxNumOfDownloads)
    {
        m_maxNumOfDownloads = maxNumOfDownloads;
    }

    const std::vector<Download>& DownloadManager::getDownloads() const
    {
        return m_downloads;
    }

    bool DownloadManager::isQueueFull() const
    {
        return m_downloads.size() == m_maxNumOfDownloads;
    }

    size_t DownloadManager::getQueueCount() const
    {
        return m_downloads.size();
    }

    bool DownloadManager::addToQueue(const Download& download)
    {
        if(m_downloads.size() != m_maxNumOfDownloads)
        {
            m_downloads.push_back(download);
            return true;
        }
        return false;
    }

    void DownloadManager::removeFromQueue(int index)
    {
        m_downloads.erase(m_downloads.begin() + index);
    }

    void DownloadManager::removeAllFromQueue()
    {
        m_downloads.clear();
    }

    int DownloadManager::downloadAll()
    {
        int success = 0;
        for(const Download& download : m_downloads)
        {
            if(download.download())
            {
                success++;
            }
            m_downloads.erase(m_downloads.begin());
        }
        return success;
    }
}
