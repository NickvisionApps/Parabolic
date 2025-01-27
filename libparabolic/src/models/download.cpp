#include "models/download.h"
#include <cmath>
#include <thread>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/gettext.h>
#include <libnick/system/environment.h>

using namespace Nickvision::Events;
using namespace Nickvision::Helpers;
using namespace Nickvision::System;
using namespace Nickvision::TubeConverter::Shared::Events;

namespace Nickvision::TubeConverter::Shared::Models
{
    static int s_downloadIdCounter{ 0 };

    static double getAriaSizeAsB(const std::string& size)
    {
        static constexpr double pow2{ 1024 * 1024 };
        static constexpr double pow3{ 1024 * 1024 * 1024 };
        size_t index;
        if((index = size.find("B")) != std::string::npos)
        {
            return std::stod(size.substr(0, index));
        }
        else if((index = size.find("KiB")) != std::string::npos)
        {
            return std::stod(size.substr(0, index)) * 1024;
        }
        else if((index = size.find("MiB")) != std::string::npos)
        {
            return std::stod(size.substr(0, index)) * pow2;
        }
        else if((index = size.find("GiB")) != std::string::npos)
        {
            return std::stod(size.substr(0, index)) * pow3;
        }
        return 0.0;
    }

    Download::Download(const DownloadOptions& options)
        : m_id{ ++s_downloadIdCounter }, 
        m_options{ options },
        m_status{ DownloadStatus::Queued },
        m_path{ m_options.getSaveFolder() / (m_options.getSaveFilename() + m_options.getFileType().getDotExtension()) },
        m_process{ nullptr }
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

    Event<DownloadCompletedEventArgs>& Download::completed()
    {
        return m_completed;
    }

    int Download::getId()
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        return m_id;
    }

    const std::string& Download::getUrl() const
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        return m_options.getUrl();
    }

    const DownloadOptions& Download::getOptions() const
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        return m_options;
    }

    DownloadStatus Download::getStatus() const
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        return m_status;
    }

    const std::filesystem::path& Download::getPath() const
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        return m_path;
    }

    const std::string& Download::getLog() const
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        if(m_process)
        {
            return m_process->getOutput();
        }
        static std::string empty;
        return empty;
    }

    const std::string& Download::getCommand() const
    {
        return m_command;
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
            m_progressChanged.invoke({ m_id, 1.0, 0.0, _("ERROR: The file already exists and overwriting is disabled.") });
            m_completed.invoke({ m_id, m_status, m_path, false });
            return;
        }
        std::vector<std::string> arguments{ m_options.toArgumentVector(downloaderOptions) };
        m_process = std::make_shared<Process>(Environment::findDependency("yt-dlp"), arguments);
        m_command = Environment::findDependency("yt-dlp").string() + " " + StringHelpers::join(arguments, " ");
        m_process->exited() += [this](const ProcessExitedEventArgs& args) { onProcessExit(args); };
        m_process->start();
        m_status = DownloadStatus::Running;
        lock.unlock();
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
        double oldProgress{ std::nan("") };
        double oldSpeed{ 0 };
        std::string oldLog{ _("Starting download...") };
        while(m_process->isRunning())
        {
            if(m_process->getOutput() != oldLog)
            {
                oldLog = m_process->getOutput();
                std::vector<std::string> logLines{ StringHelpers::split(oldLog, "\n") };
                for(size_t i = logLines.size(); i > 0; i--)
                {
                    const std::string& line{ logLines[i - 1] };
                    if((line.find("PROGRESS;") == std::string::npos && line.find("[#") == std::string::npos) || line.find("[debug]") != std::string::npos)
                    {
                        continue;
                    }
                    if(line.find("[#") != std::string::npos) //aria2c progress
                    {
                        std::vector<std::string> progress{ StringHelpers::split(line, " ") };
                        if(progress.size() != 5)
                        {
                            continue;
                        }
                        std::vector<std::string> progressSizes{ StringHelpers::split(progress[1], "/") };
                        if(progressSizes.size() != 2)
                        {
                            continue;
                        }
                        oldProgress = getAriaSizeAsB(progressSizes[0]) / getAriaSizeAsB(progressSizes[1]);
                        oldSpeed = getAriaSizeAsB(progress[3].substr(3));
                        break;
                    }
                    else
                    {
                        std::vector<std::string> progress{ StringHelpers::split(line, ";") };
                        if(progress.size() != 6 || progress[1] == "NA")
                        {
                            continue;
                        }
                        if(progress[1] == "finished" || progress[1] == "processing")
                        {
                            oldProgress = std::nan("");
                            oldSpeed = 0.0;
                        }
                        else
                        {
                            oldProgress = (progress[2] != "NA" ? std::stod(progress[2]) : 0.0) / (progress[3] != "NA" ? std::stod(progress[3]) : (progress[4] != "NA" ? std::stod(progress[4]) : 0.0));
                            oldSpeed = progress[5] != "NA" ? std::stod(progress[5]) : 0.0;
                        }
                        break;
                    }
                }
            }
            m_progressChanged.invoke({ m_id, oldProgress, oldSpeed, oldLog });
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void Download::onProcessExit(const ProcessExitedEventArgs& args)
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        if(m_status != DownloadStatus::Stopped)
        {
            m_status = args.getExitCode() == 0 ? DownloadStatus::Success : DownloadStatus::Error;
        }
        //Get final path (last line of log)
        if(m_status == DownloadStatus::Success)
        {
            std::vector<std::string> logLines{ StringHelpers::split(args.getOutput(), "\n") };
            try
            {
                m_path = logLines[logLines.size() - 1];
            }
            catch(...) { }
        }
        lock.unlock();
        m_progressChanged.invoke({ m_id, 1.0, 0.0, args.getOutput() });
        m_completed.invoke({ m_id, m_status, m_path, true });
    }
}
