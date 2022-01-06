#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <string>
#include "filetype.h"

namespace NickvisionTubeConverter::Models
{
    class Download
    {
    public:
        Download(const std::string& videoUrl, FileType fileType, const std::string& saveFolder, const std::string& newFilename);
        const std::string& getVideoUrl() const;
        FileType getFileType() const;
        const std::string& getPath() const;
        bool download() const;

    private:
        std::string m_videoUrl;
        FileType m_fileType;
        std::string m_path;
    };
}

#endif // DOWNLOAD_H
