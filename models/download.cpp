#include "download.h"
#include <cstdio>
#include <array>

using namespace NickvisionTubeConverter::Models;

Download::Download(const std::string& videoUrl, const MediaFileType& fileType, const std::string& saveFolder, const std::string& newFilename) : m_videoUrl{videoUrl}, m_fileType{fileType}, m_path{saveFolder + "/" + newFilename}
{

}

const std::string& Download::getVideoUrl() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_videoUrl;
}

const MediaFileType& Download::getMediaFileType() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_fileType;
}

std::string Download::getPath() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_path + m_fileType.toDotExtension();
}

std::pair<bool, std::string> Download::download() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    std::string cmdDownload{""};
    if(m_fileType.isVideo())
    {
        cmdDownload = "yt-dlp -f 'bv*+ba' --format " + m_fileType.toString() + " \"" + m_videoUrl + "\" -o \"" + m_path + ".%(ext)s\"";
    }
    else
    {
        cmdDownload = "yt-dlp -f 'ba' -x --audio-format " + m_fileType.toString() + " \"" + m_videoUrl + "\" -o \"" + m_path + ".%(ext)s\"";
    }
    std::string cmdOutput{"===Starting Download===\nURL: " + m_videoUrl + "\nPath: " + m_path + "\n\n"};
    std::array<char, 128> buffer;
    FILE* pipe = popen(cmdDownload.c_str(), "r");
    if(!pipe)
    {
        return std::make_pair(false, "Error: Unable to run command");
    }
    while (!feof(pipe))
    {
        if (fgets(buffer.data(), 128, pipe) != nullptr)
        {
            cmdOutput += buffer.data();
        }
    }
    int resultCode{pclose(pipe)};
    if(resultCode != EXIT_SUCCESS)
    {
        cmdOutput += "ERROR: UNABLE TO DOWNLOAD VIDEO\n";
    }
    cmdOutput += "==========\n\n";
    return std::make_pair(resultCode == EXIT_SUCCESS, cmdOutput);
}
