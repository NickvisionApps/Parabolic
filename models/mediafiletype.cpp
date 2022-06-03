#include "mediafiletype.h"
#include <algorithm>

using namespace NickvisionTubeConverter::Models;

MediaFileType::MediaFileType(Value fileType) : m_value{fileType}
{

}

std::optional<MediaFileType> MediaFileType::parse(const std::string& s)
{
    std::string sToLower{s};
    std::transform(sToLower.begin(), sToLower.end(), sToLower.begin(), ::tolower);
    if(sToLower.find(".mp4") != std::string::npos)
    {
        return MediaFileType::MP4;
    }
    else if(sToLower.find(".webm") != std::string::npos)
    {
        return MediaFileType::WEBM;
    }
    else if(sToLower.find(".mp3") != std::string::npos)
    {
        return MediaFileType::MP3;
    }
    else if(sToLower.find(".ogg") != std::string::npos)
    {
        return MediaFileType::OGG;
    }
    else if(sToLower.find(".flac") != std::string::npos)
    {
        return MediaFileType::FLAC;
    }
    else if(sToLower.find(".wma") != std::string::npos)
    {
        return MediaFileType::WMA;
    }
    else if(sToLower.find(".wav") != std::string::npos)
    {
        return MediaFileType::WAV;
    }
    else
    {
        return std::nullopt;
    }
}

MediaFileType::operator Value() const
{
    return m_value;
}

bool MediaFileType::operator==(const MediaFileType& toCompare) const
{
    return m_value == toCompare.m_value;
}

bool MediaFileType::operator!=(const MediaFileType& toCompare) const
{
    return m_value != toCompare.m_value;
}

bool MediaFileType::operator==(MediaFileType::Value toCompare) const
{
    return m_value == toCompare;
}

bool MediaFileType::operator!=(MediaFileType::Value toCompare) const
{
    return m_value != toCompare;
}

std::string MediaFileType::toString() const
{
    switch(m_value)
    {
        case MediaFileType::MP4:
            return "mp4";
        case MediaFileType::WEBM:
            return "webm";
        case MediaFileType::MP3:
            return "mp3";
        case MediaFileType::OGG:
            return "ogg";
        case MediaFileType::FLAC:
            return "flac";
        case MediaFileType::WMA:
            return "wma";
        case MediaFileType::WAV:
            return "wav";
        default:
            return "";
    }
}

std::string MediaFileType::toDotExtension() const
{
    switch(m_value)
    {
        case MediaFileType::MP4:
            return ".mp4";
        case MediaFileType::WEBM:
            return ".webm";
        case MediaFileType::MP3:
            return ".mp3";
        case MediaFileType::OGG:
            return ".ogg";
        case MediaFileType::FLAC:
            return ".flac";
        case MediaFileType::WMA:
            return ".wma";
        case MediaFileType::WAV:
            return ".wav";
        default:
            return "";
    }
}

bool MediaFileType::isAudio() const
{
    switch(m_value)
    {
        case MediaFileType::MP4:
            return false;
        case MediaFileType::WEBM:
            return false;
        case MediaFileType::MP3:
            return true;
        case MediaFileType::OGG:
            return true;
        case MediaFileType::FLAC:
            return true;
        case MediaFileType::WMA:
            return true;
        case MediaFileType::WAV:
            return true;
        default:
            return false;
    }
}

bool MediaFileType::isVideo() const
{
    switch(m_value)
    {
        case MediaFileType::MP4:
            return true;
        case MediaFileType::WEBM:
            return true;
        case MediaFileType::MP3:
            return false;
        case MediaFileType::OGG:
            return false;
        case MediaFileType::FLAC:
            return false;
        case MediaFileType::WMA:
            return false;
        case MediaFileType::WAV:
            return false;
        default:
            return false;
    }
}
