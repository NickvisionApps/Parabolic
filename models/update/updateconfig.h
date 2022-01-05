#ifndef UPDATECONFIG_H
#define UPDATECONFIG_H

#include <string>
#include <optional>
#include "version.h"

namespace NickvisionTubeConverter::Models::Update
{
    class UpdateConfig
    {
    public:
        static std::optional<UpdateConfig> loadFromUrl(const std::string& url);
        const Version& getLatestVersion() const;
        void setLatestVersion(const Version& latestVersion);
        const std::string& getChangelog() const;
        void setChangelog(const std::string& changelog);
        const std::string& getLinkToExe() const;
        void setLinkToExe(const std::string& linkToExe);

    private:
        UpdateConfig();
        Version m_latestVersion;
        std::string m_changelog;
        std::string m_linkToExe;
    };
}

#endif // UPDATECONFIG_H
