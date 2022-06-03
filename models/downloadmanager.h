#pragma once

#include <mutex>
#include <memory>
#include <vector>
#include "download.h"

namespace NickvisionTubeConverter::Models
{
    class DownloadManager
    {
    public:
        DownloadManager();
        unsigned int getMaxNumOfDownloads() const;
        void setMaxNumOfDownloads(unsigned int maxNumOfDownloads);
        const std::vector<std::shared_ptr<Download>>& getDownloads() const;
        const std::string& getLog() const;
        unsigned int getSuccessfulDownloads() const;
        bool isQueueFull() const;
        size_t getQueueCount() const;
        bool addToQueue(const std::shared_ptr<Download>& download);
        bool removeFromQueue(std::size_t index);
        void clearQueue();
        unsigned int downloadAll();

    private:
        mutable std::mutex m_mutex;
        unsigned int m_maxNumOfDownloads;
        std::vector<std::shared_ptr<Download>> m_downloads;
        std::string m_log;
        unsigned int m_successfulDownloads;
    };
}