#pragma once

#include <string>
#include <optional>

namespace NickvisionTubeConverter::Models
{
    class MediaFileType
    {
    public:
        enum Value
        {
            MP4,
            WEBM,
            MP3,
            OGG,
            FLAC
        };

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
        MediaFileType();
        Value m_value;
    };
}