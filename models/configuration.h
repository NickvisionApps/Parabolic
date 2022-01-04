#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>

namespace NickvisionTubeConverter::Models
{
    class Configuration
    {
    public:
        Configuration();
        int getMaxNumberOfActiveDownloads() const;
        void setMaxNumberOfActiveDownloads(int maxNumberOfActiveDownloads);
        const std::string& getPreviousSaveFolder() const;
        void setPreviousSaveFolder(const std::string& previousSaveFolder);
        int getPreviousFileFormat() const;
        void setPreviousFileFormat(int previousFileFormat);
        void save() const;

    private:
        std::string m_configDir;
        int m_maxNumberOfActiveDownloads;
        std::string m_previousSaveFolder;
        int m_previousFileFormat;
    };
}

#endif // CONFIGURATION_H
