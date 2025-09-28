#include "models/download.h"
#include <cmath>
#include <thread>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/gettext.h>

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
        if((index = size.find("GiB")) != std::string::npos)
        {
            return std::stod(size.substr(0, index)) * pow3;
        }
        else if((index = size.find("MiB")) != std::string::npos)
        {
            return std::stod(size.substr(0, index)) * pow2;
        }
        else if((index = size.find("KiB")) != std::string::npos)
        {
            return std::stod(size.substr(0, index)) * 1024;
        }
        else if((index = size.find("B")) != std::string::npos)
        {
            return std::stod(size.substr(0, index));
        }
        return 0.0;
    }

    static int getAriaEtaAsSeconds(std::string eta)
    {
        int hours{ 0 };
        int minutes{ 0 };
        int seconds{ 0 };
        size_t index;
        if((index = eta.find("h")) != std::string::npos)
        {
            hours = std::stoi(eta.substr(0, index)) * 3600;
            eta = eta.substr(index + 1);
        }
        if((index = eta.find("m")) != std::string::npos)
        {
            minutes = std::stoi(eta.substr(0, index)) * 60;
            eta = eta.substr(index + 1);
        }
        if((index = eta.find("s")) != std::string::npos)
        {
            seconds = std::stoi(eta.substr(0, index));
        }
        return hours + minutes + seconds;
    }

    static bool processAriaLine(const std::string& line, double& progress, double& speed, int& eta)
    {
        if(line.find("[download]") != std::string::npos)
        {
            progress = std::nan("");
            speed = 0.0;
            eta = 0;
            return true;
        }
        std::vector<std::string> fields{ StringHelpers::split(line, " ", false) };
        if(fields.size() != 5)
        {
            return false;
        }
        std::vector<std::string> sizes{ StringHelpers::split(fields[1], "/") };
        if(sizes.size() != 2)
        {
            return false;
        }
        progress = getAriaSizeAsB(sizes[0]) / getAriaSizeAsB(sizes[1]);
        speed = getAriaSizeAsB(fields[3].substr(3));
        eta = getAriaEtaAsSeconds(fields[4].substr(4, fields[4].size() - 4 - 1));
        return true;
    }

    Download::Download(const DownloadOptions& options)
        : m_id{ s_downloadIdCounter++ },
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

    void Download::start(const std::filesystem::path& ytdlpExecutable, const DownloaderOptions& downloaderOptions)
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        if(m_status == DownloadStatus::Running || m_status == DownloadStatus::Paused)
        {
            return;
        }
        if(std::filesystem::exists(m_path) && !downloaderOptions.getOverwriteExistingFiles())
        {
            m_status = DownloadStatus::Error;
            lock.unlock();
            m_progressChanged.invoke({ m_id, _("ERROR: The file already exists and overwriting is disabled.") });
            m_completed.invoke({ m_id, m_status, m_path, false });
            return;
        }
        std::vector<std::string> arguments{ m_options.toArgumentVector(downloaderOptions) };
        m_process = std::make_shared<Process>(ytdlpExecutable, arguments);
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

    void Download::pause()
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        if(m_status != DownloadStatus::Running)
        {
            return;
        }
        if(m_process->pause())
        {
            m_status = DownloadStatus::Paused;
        }
    }

    void Download::resume()
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        if(m_status != DownloadStatus::Paused)
        {
            return;
        }
        if(m_process->resume())
        {
            m_status = DownloadStatus::Running;
        }
    }

    void Download::watch()
    {
        if(!m_process)
        {
            return;
        }
        double progress{ std::nan("") };
        double speed{ 0 };
        int eta{ 0 };
        size_t logSize{ 0 };
        m_progressChanged.invoke({ m_id, _("Starting download..."), progress, speed, eta });
        while(m_process->getState() == ProcessState::Running || m_process->getState() == ProcessState::Paused)
        {
            if(m_process->getState() == ProcessState::Running)
            {
                if(m_process->getOutput().size() > logSize)
                {
                    logSize = m_process->getOutput().size();
                    std::vector<std::string> logLines{ StringHelpers::split(m_process->getOutput(), '\n', false) };
                    for(size_t i = logLines.size(); i > 0; i--)
                    {
                        try
                        {
                            const std::string& line{ logLines[i - 1] };
                            if((line.find("PROGRESS;") == std::string::npos && line.find("[#") == std::string::npos) || line.find("[debug") != std::string::npos)
                            {
                                continue;
                            }
                            //aria2c progress
                            if(line.find("[#") != std::string::npos)
                            {
#ifdef _WIN32
                                std::vector<std::string> ariaLines{ StringHelpers::split(line, '\r', false) };
                                for(size_t j = ariaLines.size(); j > 0; j--)
                                {
                                    if(processAriaLine(ariaLines[j - 1], progress, speed, eta))
                                    {
                                        break;
                                    }
                                }
                                break;
#else
                                if(processAriaLine(line, progress, speed, eta))
                                {
                                    break;
                                }
#endif
                            }
                            //yt-dlp progress
                            else
                            {
                                std::vector<std::string> fields{ StringHelpers::split(line, ";", false) };
                                if(fields.size() != 7 || fields[1] == "NA")
                                {
                                    continue;
                                }
                                if(fields[1] == "finished" || fields[1] == "processing")
                                {
                                    progress = std::nan("");
                                    speed = 0.0;
                                    eta = 0;
                                }
                                else
                                {
                                    progress = (fields[2] != "NA" ? std::stod(fields[2]) : 0.0) / (fields[3] != "NA" ? std::stod(fields[3]) : (fields[4] != "NA" ? std::stod(fields[4]) : 0.0));
                                    speed = fields[5] != "NA" ? std::stod(fields[5]) : 0.0;
                                    eta = fields[6] == "NA" || fields[6] == "Unknown" ? -1 : std::stoi(fields[6]);
                                }
                                break;
                            }
                        }
                        catch(...)
                        {
                            continue;
                        }
                    }
                }
                m_progressChanged.invoke({ m_id, m_process->getOutput(), progress, speed, eta });
            }
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
            std::vector<std::string> logLines{ StringHelpers::split(m_process->getOutput(), "\n") };
            try
            {
                std::filesystem::path finalPath{ logLines[logLines.size() - 1] };
                if(!std::filesystem::exists(finalPath))
                {
                    finalPath = logLines[logLines.size() - 2];
                }
                if(std::filesystem::exists(finalPath))
                {
                    m_path = finalPath;
                }
            }
            catch(...) { }
        }
        lock.unlock();
        m_progressChanged.invoke({ m_id, m_process->getOutput() });
        m_completed.invoke({ m_id, m_status, m_path, true });
    }
}
