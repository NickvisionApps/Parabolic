#pragma once

#include <mutex>
#include <string>
#include <utility>
#include "mediafiletype.h"

namespace NickvisionTubeConverter::Models
{
    class Download
    {
    public:
        Download(const std::string& videoUrl, const MediaFileType& fileType, const std::string& saveFolder, const std::string& newFilename);
        const std::string& getVideoUrl() const;
        const MediaFileType& getMediaFileType() const;
        std::string getPath() const;
        std::pair<bool, std::string> download() const;

    private:
        mutable std::mutex m_mutex;
        std::string m_videoUrl;
        MediaFileType m_fileType;
        std::string m_path;
    };
}