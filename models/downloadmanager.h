#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <vector>
#include "download.h"

namespace NickvisionTubeConverter::Models
{
    class DownloadManager
    {
    public:
        DownloadManager();
        int getMaxNumOfDownloads() const;
        void setMaxNumOfDownloads(int maxNumOfDownloads);
        const std::vector<Download>& getDownloads() const;
        bool isQueueFull() const;
        size_t getQueueCount() const;
        bool addToQueue(const Download& download);
        void removeFromQueue(int index);
        void removeAllFromQueue();
        int downloadAll();

    private:
        int m_maxNumOfDownloads;
        std::vector<Download> m_downloads;
    };
}

#endif // DOWNLOADMANAGER_H
