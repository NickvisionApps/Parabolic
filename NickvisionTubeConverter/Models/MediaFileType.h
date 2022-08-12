#pragma once

#include <optional>
#include <string>

namespace NickvisionTubeConverter::Models
{
    class MediaFileType
    {
    public:
        enum Value
        {
            MP4 = 0,
            WEBM,
            MP3,
            OPUS,
            FLAC,
            WAV
        };

        MediaFileType() = delete;
        MediaFileType(Value fileType);
        static std::optional<MediaFileType> parse(const std::string& s);
        operator Value() const;
        explicit operator bool() = delete;
        bool operator==(const MediaFileType& toCompare) const;
        bool operator!=(const MediaFileType& toCompare) const;
        bool operator==(MediaFileType::Value toCompare) const;
        bool operator!=(MediaFileType::Value toCompare) const;
        std::string toString() const;
        std::string toDotExtension() const;
        bool isAudio() const;
        bool isVideo() const;

    private:
        Value m_value;
    };
}

