#include "updater.h"
#include <filesystem>
#include <fstream>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

namespace NickvisionTubeConverter::Models::Update
{
    Updater::Updater(const std::string& linkToConfig, const Version& currentVersion) : m_linkToConfig(linkToConfig), m_currentVersion(currentVersion), m_updateConfig(std::nullopt), m_updateAvailable(false)
    {

    }

    bool Updater::updateAvailable() const
    {
        return m_updateAvailable && m_updateConfig.has_value();
    }

    std::optional<Version> Updater::getLatestVersion() const
    {
        if (m_updateConfig.has_value())
        {
            return m_updateConfig->getLatestVersion();
        }
        return std::nullopt;
    }

    std::string Updater::getChangelog() const
    {
        return m_updateConfig.has_value() ? m_updateConfig->getChangelog() : "";
    }

    bool Updater::checkForUpdates()
    {
        m_updateConfig = UpdateConfig::loadFromUrl(m_linkToConfig);
        if (m_updateConfig.has_value() && m_updateConfig->getLatestVersion() > m_currentVersion)
        {
            m_updateAvailable = true;
        }
        return updateAvailable();
    }

    bool Updater::update()
    {
        if(!updateAvailable())
        {
            return false;
        }
        std::string downloadsDir = std::string(getpwuid(getuid())->pw_dir) + "/Downloads";
        if(std::filesystem::exists(downloadsDir))
        {
            std::filesystem::create_directories(downloadsDir);
        }
        std::string exePath = downloadsDir + "/NickvisionTubeConverter";
        std::ofstream exeFile(exePath, std::ios::out | std::ios::trunc | std::ios::binary);
        if(exeFile.is_open())
        {
            cURLpp::Easy handle;
            try
            {
                handle.setOpt(cURLpp::Options::Url(m_updateConfig->getLinkToExe()));
                handle.setOpt(cURLpp::Options::FollowLocation(true));
                handle.setOpt(cURLpp::Options::WriteStream(&exeFile));
                handle.perform();
            }
            catch(...)
            {
                return false;
            }
            exeFile.close();
        }
        else
        {
            return false;
        }
        chmod(exePath.c_str(), S_IRWXU);
        return true;
    }
}
