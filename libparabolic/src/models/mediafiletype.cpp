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
        if(value == "video")
        {
            return { MediaFileTypeValue::Video };
        }
        else if(value == "mp4")
        {
            return { MediaFileTypeValue::MP4 };
        }
        else if(value == "webm")
        {
            return { MediaFileTypeValue::WEBM };
        }
        else if(value == "mkv")
        {
            return { MediaFileTypeValue::MKV };
        }
        else if(value == "mov")
        {
            return { MediaFileTypeValue::MOV };
        }
        else if(value == "avi")
        {
            return { MediaFileTypeValue::AVI };
        }
        else if(value == "audio")
        {
            return { MediaFileTypeValue::Audio };
        }
        else if(value == "mp3")
        {
            return { MediaFileTypeValue::MP3 };
        }
        else if(value == "m4a")
        {
            return { MediaFileTypeValue::M4A };
        }
        else if(value == "opus")
        {
            return { MediaFileTypeValue::OPUS };
        }
        else if(value == "flac")
        {
            return { MediaFileTypeValue::FLAC };
        }
        else if(value == "wav")
        {
            return { MediaFileTypeValue::WAV };
        }
        else if(value == "ogg" || value == "vorbis")
        {
            return { MediaFileTypeValue::OGG };
        }
        return std::nullopt;
    }

    int MediaFileType::getVideoFileTypeCount()
    {
        return 6;
    }

    int MediaFileType::getAudioFileTypeCount()
    {
        return 6;
    }

    std::string MediaFileType::getDotExtension() const
    {
        if(m_value == MediaFileTypeValue::Video || m_value == MediaFileTypeValue::Audio)
        {
            return "";
        }
        return "." + StringHelpers::lower(str());
    }

    bool MediaFileType::isAudio() const
    {
        switch (m_value)
        {
        case MediaFileTypeValue::Audio:
        case MediaFileTypeValue::MP3:
        case MediaFileTypeValue::M4A:
        case MediaFileTypeValue::OPUS:
        case MediaFileTypeValue::FLAC:
        case MediaFileTypeValue::WAV:
        case MediaFileTypeValue::OGG:
            return true;
        default:
            return false;
        }
    }

    bool MediaFileType::isVideo() const
    {
        switch (m_value)
        {
        case MediaFileTypeValue::Video:
        case MediaFileTypeValue::MP4:
        case MediaFileTypeValue::WEBM:
        case MediaFileTypeValue::MKV:
        case MediaFileTypeValue::MOV:
        case MediaFileTypeValue::AVI:
            return true;
        default:
            return false;
        }
    }

    bool MediaFileType::isGeneric() const
    {
        switch (m_value)
        {
        case MediaFileType::Video:
        case MediaFileType::Audio:
            return true;
        default:
            return false;
        }
    }

    bool MediaFileType::supportsThumbnails() const
    {
        switch (m_value)
        {
        case MediaFileTypeValue::MP4:
        case MediaFileTypeValue::MKV:
        case MediaFileTypeValue::MOV:
        case MediaFileTypeValue::MP3:
        case MediaFileTypeValue::M4A:
        case MediaFileTypeValue::OPUS:
        case MediaFileTypeValue::FLAC:
        case MediaFileTypeValue::OGG:
            return true;
        default:
            return false;
        }
    }

    bool MediaFileType::supportsSubtitleFormat(SubtitleFormat format) const
    {
        switch(format)
        {
        case SubtitleFormat::Any:
            return isVideo() && m_value != MediaFileTypeValue::AVI;
        case SubtitleFormat::VTT:
            return isVideo() && m_value != MediaFileTypeValue::AVI;
        case SubtitleFormat::SRT:
            return isVideo() && m_value != MediaFileTypeValue::WEBM && m_value != MediaFileTypeValue::AVI;
        case SubtitleFormat::ASS:
            return m_value == MediaFileTypeValue::MKV;
        case SubtitleFormat::LRC:
            return isAudio();
        default:
            return false;
        }

    }

    bool MediaFileType::shouldRecode() const
    {
        switch(m_value)
        {
        case MediaFileTypeValue::WEBM:
        case MediaFileTypeValue::MOV:
        case MediaFileTypeValue::AVI:
            return true;
        default:
            return false;
        }
    }

    std::string MediaFileType::str() const
    {
        switch (m_value)
        {
        case MediaFileTypeValue::Video:
            return "Video";
        case MediaFileTypeValue::MP4:
            return "MP4";
        case MediaFileTypeValue::WEBM:
            return "WEBM";
        case MediaFileTypeValue::MKV:
            return "MKV";
        case MediaFileTypeValue::MOV:
            return "MOV";
        case MediaFileTypeValue::AVI:
            return "AVI";
        case MediaFileTypeValue::Audio:
            return "Audio";
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
        case MediaFileTypeValue::OGG:
            return "OGG";
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

    MediaFileType::operator size_t() const
    {
        return static_cast<size_t>(m_value);
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
