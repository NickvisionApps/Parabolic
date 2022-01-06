#include "filetypehelpers.h"
#include <algorithm>

namespace NickvisionTubeConverter::Helpers
{
    using namespace NickvisionTubeConverter::Models;

    std::string FileTypeHelpers::toString(FileType fileType)
    {
        switch(fileType)
        {
            case FileType::MP4:
                return "MP4";
            case FileType::WEBM:
                return "WEBM";
            case FileType::MP3:
                return "MP3";
            case FileType::WAV:
                return "WAV";
            case FileType::FLAC:
                return "FLAC";
        }
    }

    std::string FileTypeHelpers::toDotExtension(FileType fileType)
    {
        switch(fileType)
        {
            case FileType::MP4:
                return ".mp4";
            case FileType::WEBM:
                return ".webm";
            case FileType::MP3:
                return ".mp3";
            case FileType::WAV:
                return ".wav";
                return ".ogg";
            case FileType::FLAC:
                return ".flac";
        }
    }

    FileType FileTypeHelpers::parse(const std::string& s)
    {
        std::string sToLower = s;
        std::transform(sToLower.begin(), sToLower.end(), sToLower.begin(), ::tolower);
        if(sToLower.find("mp4") != std::string::npos)
        {
            return FileType::MP4;
        }
        else if(sToLower.find("webm") != std::string::npos)
        {
            return FileType::WEBM;
        }
        else if(sToLower.find("mp3") != std::string::npos)
        {
            return FileType::MP3;
        }
        else if(sToLower.find("wav") != std::string::npos)
        {
            return FileType::WAV;
        }
        else if(sToLower.find("flac") != std::string::npos)
        {
            return FileType::FLAC;
        }
        else
        {
            throw std::invalid_argument("Unable to parse. String does not contain a supported file type.");
        }
    }

    bool FileTypeHelpers::isAudio(FileType fileType)
    {
        switch(fileType)
        {
            case FileType::MP4:
                return false;
            case FileType::WEBM:
                return false;
            case FileType::MP3:
                return true;
            case FileType::WAV:
                return true;
            case FileType::FLAC:
                return true;
        }
    }

    bool FileTypeHelpers::isVideo(FileType fileType)
    {
        switch(fileType)
        {
            case FileType::MP4:
                return true;
            case FileType::WEBM:
                return true;
            case FileType::MP3:
                return false;
            case FileType::WAV:
                return false;
            case FileType::FLAC:
                return false;
        }
    }
}
