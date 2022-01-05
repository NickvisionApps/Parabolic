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
            case FileType::MOV:
                return "MOV";
            case FileType::AVI:
                return "AVI";
            case FileType::MP3:
                return "MP3";
            case FileType::WAV:
                return "WAV";
            case FileType::WMA:
                return "WMA";
            case FileType::OGG:
                return "OGG";
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
            case FileType::MOV:
                return ".mov";
            case FileType::AVI:
                return ".avi";
            case FileType::MP3:
                return ".mp3";
            case FileType::WAV:
                return ".wav";
            case FileType::WMA:
                return ".wma";
            case FileType::OGG:
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
        else if(sToLower.find("mov") != std::string::npos)
        {
            return FileType::MOV;
        }
        else if(sToLower.find("avi") != std::string::npos)
        {
            return FileType::AVI;
        }
        else if(sToLower.find("mp3") != std::string::npos)
        {
            return FileType::MP3;
        }
        else if(sToLower.find("wav") != std::string::npos)
        {
            return FileType::WAV;
        }
        else if(sToLower.find("wma") != std::string::npos)
        {
            return FileType::WMA;
        }
        else if(sToLower.find("ogg") != std::string::npos)
        {
            return FileType::OGG;
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
            case FileType::MOV:
                return false;
            case FileType::AVI:
                return false;
            case FileType::MP3:
                return true;
            case FileType::WAV:
                return true;
            case FileType::WMA:
                return true;
            case FileType::OGG:
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
            case FileType::MOV:
                return true;
            case FileType::AVI:
                return true;
            case FileType::MP3:
                return false;
            case FileType::WAV:
                return false;
            case FileType::WMA:
                return false;
            case FileType::OGG:
                return false;
            case FileType::FLAC:
                return false;
        }
    }
}
