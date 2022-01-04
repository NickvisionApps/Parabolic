#ifndef UPDATER_H
#define UPDATER_H

#include <string>
#include <optional>
#include "version.h"
#include "updateconfig.h"

namespace NickvisionTubeConverter::Models::Update
{
    class Updater
    {
    public:
        Updater(const std::string& linkToConfig, const Version& currentVersion);
        bool updateAvailable() const;
        std::optional<Version> getLatestVersion() const;
        std::string getChangelog() const;
        bool checkForUpdates();

    private:
        std::string m_linkToConfig;
        Version m_currentVersion;
        std::optional<UpdateConfig> m_updateConfig;
        bool m_updateAvailable;
    };
}


#endif // UPDATER_H
