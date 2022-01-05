#include "ytdlpmanager.h"
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

namespace NickvisionTubeConverter::Models
{
    using namespace NickvisionTubeConverter::Models::Update;

    YtDlpManager::YtDlpManager() : m_exePath(std::string(getpwuid(getuid())->pw_dir) + "/.config/Nickvision/NickvisionTubeConverter/yt-dlp"), m_latestVersion("2021.12.27"), m_currentVersion("0.0.0")
    {

    }

    const std::string& YtDlpManager::getExePath() const
    {
        return m_exePath;
    }

    const Version& YtDlpManager::getLatestVersion() const
    {
        return m_latestVersion;
    }

    void YtDlpManager::setCurrentVersion(const Update::Version& currentVersion)
    {
        m_currentVersion = currentVersion;
    }

    void YtDlpManager::downloadLatestVersionIfNeeded() const
    {
        if(!std::filesystem::exists(m_exePath) || m_latestVersion > m_currentVersion)
        {
            std::ofstream ytDlpFile(m_exePath, std::ios::out | std::ios::trunc | std::ios::binary);
            if(ytDlpFile.is_open())
            {
                cURLpp::Easy handle;
                try
                {
                    handle.setOpt(cURLpp::Options::Url("https://github.com/yt-dlp/yt-dlp/releases/download/" + m_latestVersion.toString() + "/yt-dlp"));
                    handle.setOpt(cURLpp::Options::FollowLocation(true));
                    handle.setOpt(cURLpp::Options::WriteStream(&ytDlpFile));
                    handle.perform();
                }
                catch(...)
                {
                    throw std::runtime_error("Unable to download yt-dlp file.");
                }
                ytDlpFile.close();
            }
            else
            {
                throw std::runtime_error("Unable to create yt-dlp file.");
            }
            chmod(m_exePath.c_str(), S_IRWXU);
        }
    }
}
