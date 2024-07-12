#include "models/download.h"
#include <thread>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/gettext.h>
#include <libnick/system/environment.h>

using namespace Nickvision::Events;
using namespace Nickvision::Helpers;
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
        std::thread watcher{ &Download::watch, this };
        watcher.detach();
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

    void Download::watch()
    {
        if(!m_process)
        {
            return;
        }
        std::string oldLog;
        while(m_process->isRunning())
        {
            std::string log{ m_process->getOutput() };
            std::vector<std::string> logLines{ StringHelpers::split(log, "\n") };
            for(size_t i = logLines.size(); i > 0; i--)
            {
                const std::string& line{ logLines[i - 1] };
                if(line.find("PROGRESS;") == std::string::npos)
                {
                    continue;
                }
                std::vector<std::string> progress{ StringHelpers::split(line, ";") };
                if(progress.size() != 5 || progress[1] == "NA")
                {
                    continue;
                }
                if(progress[1] == "finished" || progress[1] == "processing")
                {
                    m_progressChanged.invoke({ std::nan(""), 0.0, log });
                    return;
                }
                if(log != oldLog)
                {
                    oldLog = log;
                    m_progressChanged.invoke({ (progress[2] != "NA" ? std::stod(progress[2]) : 0) / (progress[3] != "NA" ? std::stod(progress[3]) : 1), (progress[4] != "NA" ? std::stod(progress[4]) : 0), log });
                }
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
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