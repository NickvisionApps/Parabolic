#include "file.h"
#include "string_manip.h"

namespace parabolic::file
{
    std::string_view dot_extension(Type type)
    {
        switch (type)
        {
        case Type::Mp4:
            return ".mp4";
        case Type::Webm:
            return ".webm";
        case Type::Mkv:
            return ".mkv";
        case Type::Mov:
            return ".mov";
        case Type::Avi:
            return ".avi";
        case Type::Mp3:
            return ".mp3";
        case Type::M4a:
            return ".m4a";
        case Type::Opus:
            return ".opus";
        case Type::Flac:
            return ".flac";
        case Type::Wav:
            return ".wav";
        case Type::Ogg:
            return ".ogg";
        default:
            return "";
        }
    }

    bool is_audio(Type type)
    {
        switch (type)
        {
        case Type::AudioGeneric:
        case Type::Mp3:
        case Type::M4a:
        case Type::Opus:
        case Type::Flac:
        case Type::Wav:
        case Type::Ogg:
            return true;
        default:
            return false;
        }
    }

    bool is_generic(Type type)
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

    bool is_video(Type type)
    {
        switch (type)
        {
        case Type::VideoGeneric:
        case Type::Mp4:
        case Type::Webm:
        case Type::Mkv:
        case Type::Mov:
        case Type::Avi:
            return true;
        default:
            return false;
        }
    }

    bool should_recode(Type type)
    {
        switch (type)
        {
        case Type::Webm:
        case Type::Mov:
        case Type::Avi:
            return true;
        default:
            return false;
        }
    }

    bool supports_subtitle(Type type, subtitle::Format format)
    {
        switch (format)
        {
        case subtitle::Format::Any:
        case subtitle::Format::Vtt:
            return is_video(type) && type != Type::Avi;
        case subtitle::Format::Srt:
            return is_video(type) && type != Type::Webm && type != Type::Avi;
        case subtitle::Format::Ass:
            return is_video(type) && type != Type::Mkv;
        case subtitle::Format::Lrc:
            return is_audio(type);
        default:
            return false;
        }
    }

    bool supports_thumbnails(Type type)
    {
        switch (type)
        {
        case Type::Mp4:
        case Type::Mkv:
        case Type::Mov:
        case Type::Mp3:
        case Type::M4a:
        case Type::Opus:
        case Type::Flac:
        case Type::Ogg:
            return true;
        default:
            return false;
        }
    }
}
