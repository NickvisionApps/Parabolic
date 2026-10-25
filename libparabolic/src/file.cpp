#include "file.h"
#include "string_manip.h"

namespace parabolic::file
{
    std::string dot_extension(Type type)
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
}
