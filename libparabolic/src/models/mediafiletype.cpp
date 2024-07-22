#include "models/mediafiletype.h"
#include <libnick/helpers/stringhelpers.h>

using namespace Nickvision::Helpers;

namespace Nickvision::TubeConverter::Shared::Models
{
    MediaFileType::MediaFileType(MediaFileTypeValue value)
        : m_value{ value }
    {
        
    }

    std::optional<MediaFileType> MediaFileType::parse(std::string value)
    {
        value = StringHelpers::replace(value, ".", "");
        value = StringHelpers::lower(value);
        if (value == "mp4")
        {
            return { MediaFileTypeValue::MP4 };
        }
        else if (value == "webm")
        {
            return { MediaFileTypeValue::WEBM };
        }
        else if (value == "mp3")
        {
            return { MediaFileTypeValue::MP3 };
        }
        else if (value == "m4a")
        {
            return { MediaFileTypeValue::M4A };
        }
        else if (value == "opus")
        {
            return { MediaFileTypeValue::OPUS };
        }
        else if (value == "flac")
        {
            return { MediaFileTypeValue::FLAC };
        }
        else if (value == "wav")
        {
            return { MediaFileTypeValue::WAV };
        }
        return std::nullopt;
    }

    std::string MediaFileType::getDotExtension() const
    {
        return "." + StringHelpers::lower(str());
    }

    bool MediaFileType::isAudio() const
    {
        switch (m_value)
        {
        case MediaFileTypeValue::MP3:
        case MediaFileTypeValue::M4A:
        case MediaFileTypeValue::OPUS:
        case MediaFileTypeValue::FLAC:
        case MediaFileTypeValue::WAV:
            return true;
        case MediaFileTypeValue::MP4:
        case MediaFileTypeValue::WEBM:
        default:
            return false;
        }
    }

    bool MediaFileType::isVideo() const
    {
        switch (m_value)
        {
        case MediaFileTypeValue::MP4:
        case MediaFileTypeValue::WEBM:
            return true;
        case MediaFileTypeValue::MP3:
        case MediaFileTypeValue::M4A:
        case MediaFileTypeValue::OPUS:
        case MediaFileTypeValue::FLAC:
        case MediaFileTypeValue::WAV:
        default:
            return false;
        }
    }

    bool MediaFileType::supportsThumbnails() const
    {
        switch (m_value)
        {
        case MediaFileTypeValue::MP4:
        case MediaFileTypeValue::MP3:
        case MediaFileTypeValue::M4A:
        case MediaFileTypeValue::OPUS:
        case MediaFileTypeValue::FLAC:
            return true;
        case MediaFileTypeValue::WEBM:
        case MediaFileTypeValue::WAV:
        default:
            return false;
        }
    }

    std::string MediaFileType::str() const
    {
        switch (m_value)
        {
        case MediaFileTypeValue::MP4:
            return "MP4";
        case MediaFileTypeValue::WEBM:
            return "WEBM";
        case MediaFileTypeValue::MP3:
            return "MP3";
        case MediaFileTypeValue::M4A:
            return "M4A";
        case MediaFileTypeValue::OPUS:
            return "OPUS";
        case MediaFileTypeValue::FLAC:
            return "FLAC";
        case MediaFileTypeValue::WAV:
            return "WAV";
        default:
            return "";
        }
    }

    MediaFileType::operator MediaFileTypeValue() const
    {
        return m_value;
    }

    MediaFileType::operator int() const
    {
        return static_cast<int>(m_value);
    }

    MediaFileType::operator unsigned int() const
    {
        return static_cast<unsigned int>(m_value);
    }

    bool MediaFileType::operator==(const MediaFileType& other) const
    {
        return m_value == other.m_value;
    }

    bool MediaFileType::operator!=(const MediaFileType& other) const
    {
        return !operator==(other);
    }

    bool MediaFileType::operator==(MediaFileTypeValue other) const
    {
        return m_value == other;
    }

    bool MediaFileType::operator!=(MediaFileTypeValue other) const
    {
        return !operator==(other);
    }
}