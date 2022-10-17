#include "adddownloaddialogcontroller.hpp"
#include <filesystem>

using namespace NickvisionTubeConverter::Controllers;
using namespace NickvisionTubeConverter::Models;

AddDownloadDialogController::AddDownloadDialogController(Configuration& configuration) : m_configuration{ configuration }
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

const std::shared_ptr<Download>& AddDownloadDialogController::getDownload() const
{
    return m_download;
}

DownloadCheckStatus AddDownloadDialogController::setDownload(const std::string& videoUrl, int mediaFileType, const std::string& saveFolder, const std::string& newFilename, int quality, int subtitles)
{
    m_download = std::make_shared<Download>(videoUrl, static_cast<MediaFileType::Value>(mediaFileType), saveFolder, newFilename, static_cast<Quality>(quality), static_cast<Subtitles>(subtitles));
    DownloadCheckStatus checkStatus{ m_download->getValidStatus() };
    if(checkStatus == DownloadCheckStatus::Valid)
    {
        m_configuration.setPreviousSaveFolder(std::filesystem::path(m_download->getSavePath()).parent_path().string());
        m_configuration.setPreviousFileType(m_download->getMediaFileType());
        m_configuration.save();
    }
    return checkStatus;
}

