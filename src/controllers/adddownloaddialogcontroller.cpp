#include "adddownloaddialogcontroller.hpp"
#include <filesystem>

using namespace NickvisionTubeConverter::Controllers;
using namespace NickvisionTubeConverter::Models;

AddDownloadDialogController::AddDownloadDialogController(Configuration& configuration) : m_configuration{ configuration }, m_download{ "", MediaFileType::MP4, "", "" }
{

}

const std::string& AddDownloadDialogController::getResponse() const
{
    return m_response;
}

void AddDownloadDialogController::setResponse(const std::string& response)
{
    m_response = response;
}

std::string AddDownloadDialogController::getPreviousSaveFolder() const
{
    return std::filesystem::exists(m_configuration.getPreviousSaveFolder()) ? m_configuration.getPreviousSaveFolder() : "";
}

int AddDownloadDialogController::getPreviousFileTypeAsInt() const
{
    return static_cast<int>(m_configuration.getPreviousFileType());
}

const Download& AddDownloadDialogController::getDownload() const
{
    return m_download;
}

DownloadCheckStatus AddDownloadDialogController::checkIfDownloadValid() const
{
    if(m_download.getVideoUrl().empty())
    {
        return DownloadCheckStatus::EmptyVideoUrl;
    }
    if(m_download.getVideoUrl().find("https://www.youtube.com/watch?v=") == std::string::npos && m_download.getVideoUrl().find("http://www.youtube.com/watch?v=") == std::string::npos)
    {
        return DownloadCheckStatus::InvalidVideoUrl;
    }
    std::filesystem::path downloadPath{ m_download.getSavePath() };
    if(downloadPath.parent_path() == "/")
    {
        return DownloadCheckStatus::EmptySaveFolder;
    }
    if(!std::filesystem::exists(downloadPath.parent_path()))
    {
        return DownloadCheckStatus::InvalidSaveFolder;
    }
    if(downloadPath.filename() == m_download.getMediaFileType().toDotExtension())
    {
        return DownloadCheckStatus::EmptyNewFilename;
    }
    return DownloadCheckStatus::Valid;
}

DownloadCheckStatus AddDownloadDialogController::setDownload(const std::string& videoUrl, int mediaFileType, const std::string& saveFolder, const std::string& newFilename, int quality)
{
    m_download = { videoUrl, static_cast<MediaFileType::Value>(mediaFileType), saveFolder, newFilename, static_cast<Quality>(quality) };
    DownloadCheckStatus checkStatus{ checkIfDownloadValid() };
    if(checkStatus == DownloadCheckStatus::Valid)
    {
        m_configuration.setPreviousSaveFolder(std::filesystem::path(m_download.getSavePath()).parent_path().string() + "/");
        m_configuration.setPreviousFileType(m_download.getMediaFileType());
        m_configuration.save();
    }
    return checkStatus;
}

