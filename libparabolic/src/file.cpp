#include "file.h"
#include "string_manip.h"

namespace parabolic
{
    std::string file::dot_extension(Type type)
    {
        switch (type)
        {
        case Type::MP4:
            return ".mp4";
        case Type::WEBM:
            return ".webm";
        case Type::MKV:
            return ".mkv";
        case Type::MOV:
            return ".mov";
        case Type::AVI:
            return ".avi";
        case Type::MP3:
            return ".mp3";
        case Type::M4A:
            return ".m4a";
        case Type::OPUS:
            return ".opus";
        case Type::FLAC:
            return ".flac";
        case Type::WAV:
            return ".wav";
        case Type::OGG:
            return ".ogg";
        default:
            return "";
        }
    }

    bool file::is_audio(Type type)
    {
        switch (type)
        {
        case Type::AudioGeneric:
        case Type::MP3:
        case Type::M4A:
        case Type::OPUS:
        case Type::FLAC:
        case Type::WAV:
        case Type::OGG:
            return true;
        default:
            return false;
        }
    }

    bool file::is_generic(Type type)
    {
        switch (type)
        {
        case Type::AudioGeneric:
        case Type::VideoGeneric:
            return true;
        default:
            return false;
        }
    }

    bool file::is_video(Type type)
    {
        switch (type)
        {
        case Type::VideoGeneric:
        case Type::MP4:
        case Type::WEBM:
        case Type::MKV:
        case Type::MOV:
        case Type::AVI:
            return true;
        default:
            return false;
        }
    }

    bool file::should_recode(Type type)
    {
        switch (type)
        {
        case Type::WEBM:
        case Type::MOV:
        case Type::AVI:
            return true;
        default:
            return false;
        }
    }

    bool file::supports_subtitle(Type type, subtitle::Format format)
    {
        switch (format)
        {
        case subtitle::Format::Any:
        case subtitle::Format::VTT:
            return is_video(type) && type != Type::AVI;
        case subtitle::Format::SRT:
            return is_video(type) && type != Type::WEBM && type != Type::AVI;
        case subtitle::Format::ASS:
            return is_video(type) && type != Type::MKV;
        case subtitle::Format::LRC:
            return is_audio(type);
        default:
            return false;
        }
    }

    bool file::supports_thumbnails(Type type)
    {
        switch (type)
        {
        case Type::MP4:
        case Type::MKV:
        case Type::MOV:
        case Type::MP3:
        case Type::M4A:
        case Type::OPUS:
        case Type::FLAC:
        case Type::OGG:
            return true;
        default:
            return false;
        }
    }
}
