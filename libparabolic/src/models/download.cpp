#include "models/download.h"
#include <libnick/filesystem/userdirectories.h>
#include <fstream>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/gettext.h>

using namespace Nickvision::Events;
using namespace Nickvision::Filesystem;
using namespace Nickvision::Helpers;
using namespace Nickvision::System;

namespace Nickvision::TubeConverter::Shared::Models
{
    Download::Download(const DownloadOptions& options)
        : m_options{ options },
        m_id{ StringHelpers::newGuid() },
        m_status{ DownloadStatus::NotStarted },
        m_filename{ m_options.getSaveFilename() + (m_options.getFileType().isGeneric() ? "" : m_options.getFileType().getDotExtension()) },
        m_tempDirPath{ UserDirectories::get(UserDirectory::Cache) / m_id },
        m_logFilePath{ m_tempDirPath / "log.log" }
    {
        std::filesystem::create_directories(m_tempDirPath);
    }

    Download::~Download()
    {
        stop();
        std::filesystem::remove_all(m_tempDirPath);
    }

    Event<DownloadProgressChangedEventArgs>& Download::progressChanged()
    {
        return m_progressChanged;
    }

    Event<ParamEventArgs<DownloadStatus>>& Download::completed()
    {
        return m_completed;
    }

    std::filesystem::path Download::getPath() const
    {
        return m_options.getSaveFolder() / m_filename;
    }

    DownloadStatus Download::getStatus() const
    {
        return m_status;
    }

    void Download::start(const DownloaderOptions& downloaderOptions)
    {
        if(m_status == DownloadStatus::Running)
        {
            return;
        }
        //TODO
    }

    void Download::stop()
    {
        if(m_status != DownloadStatus::Running)
        {
            return;
        }
        //TODO
    }
}