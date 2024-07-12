#include "models/download.h"
#include <libnick/localization/gettext.h>
#include <libnick/system/environment.h>

using namespace Nickvision::Events;
using namespace Nickvision::System;

namespace Nickvision::TubeConverter::Shared::Models
{
    Download::Download(const DownloadOptions& options)
        : m_options{ options },
        m_status{ DownloadStatus::NotStarted },
        m_process{ nullptr },
        m_path{ options.getSaveFolder() / (options.getSaveFilename() + options.getFileType().getDotExtension()) }
    {
        
    }

    Download::~Download()
    {
        stop();
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
        std::lock_guard<std::mutex> lock{ m_mutex };
        return m_path;
    }

    DownloadStatus Download::getStatus() const
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        return m_status;
    }

    void Download::start(const DownloaderOptions& downloaderOptions)
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        if(m_status == DownloadStatus::Running)
        {
            return;
        }
        if(std::filesystem::exists(m_path) && !downloaderOptions.getOverwriteExistingFiles())
        {
            m_status = DownloadStatus::Error;
            lock.unlock();
            m_progressChanged.invoke({ 1.0, 0.0, _("The file already exists and overwriting is disabled.") });
            m_completed.invoke({ m_status });
            return;
        }
        m_process = std::make_shared<Process>(Environment::findDependency("yt-dlp"), m_options.toArgumentVector(downloaderOptions));
        m_process->exited() += [this](const ProcessExitedEventArgs& args) { onProcessExit(args); };
        m_process->start();
        m_status = DownloadStatus::Running;
    }

    void Download::stop()
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        if(m_status != DownloadStatus::Running)
        {
            return;
        }
        if(m_process->kill())
        {
            m_status = DownloadStatus::Stopped;
        }
    }

    void Download::onProcessExit(const ProcessExitedEventArgs& args)
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        if(m_status != DownloadStatus::Stopped)
        {
            m_status = std::filesystem::exists(m_path) ? DownloadStatus::Success : DownloadStatus::Error;
        }
        lock.unlock();
        m_progressChanged.invoke({ 1.0, 0.0, args.getOutput() });
        m_completed.invoke({ m_status });
    }
}