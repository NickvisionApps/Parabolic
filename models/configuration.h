#pragma once

#include <mutex>
#include <string>
#include "mediafiletype.h"

namespace NickvisionTubeConverter::Models
{
    enum class Theme
    {
        System,
        Light,
        Dark
    };

    class Configuration
    {
    public:
        Configuration();
        Theme getTheme() const;
        void setTheme(Theme theme);
        unsigned int getMaxNumberOfActiveDownloads() const;
        void setMaxNumberOfActiveDownloads(unsigned int maxNumberOfActiveDownloads);
        const std::string& getPreviousSaveFolder() const;
        void setPreviousSaveFolder(const std::string& previousSaveFolder);
        const MediaFileType& getPreviousFileFormat() const;
        void setPreviousFileFormat(const MediaFileType& previousFileFormat);
        void save() const;

    private:
        mutable std::mutex m_mutex;
        std::string m_configDir;
        Theme m_theme;
        unsigned int m_maxNumberOfActiveDownloads;
        std::string m_previousSaveFolder;
        MediaFileType m_previousFileFormat;
    };
}