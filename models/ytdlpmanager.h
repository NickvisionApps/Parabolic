#ifndef YTDLPMANAGER_H
#define YTDLPMANAGER_H

#include "update/version.h"

namespace NickvisionTubeConverter::Models
{
    class YtDlpManager
    {
    public:
        YtDlpManager();
        const std::string& getExePath() const;
        const Update::Version& getLatestVersion() const;
        void setCurrentVersion(const Update::Version& currentVersion);
        void downloadLatestVersionIfNeeded() const;

    private:
        std::string m_exePath;
        Update::Version m_latestVersion;
        Update::Version m_currentVersion;
    };
}

#endif // YTDLPMANAGER_H
