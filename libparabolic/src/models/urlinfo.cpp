#include "models/urlinfo.h"
#include <libnick/system/environment.h>
#include <libnick/system/process.h>

using namespace Nickvision::Keyring;
using namespace Nickvision::System;

namespace Nickvision::TubeConverter::Shared::Models
{
    UrlInfo::UrlInfo(const std::string& url, boost::json::object info)
        : m_url{ url },
        m_isPlaylist{ false }
    {
        boost::json::array entries = info["entries"].is_array() ? info["entries"].as_array() : boost::json::array();
        if(!entries.empty())
        {
            m_isPlaylist = true;
            int i{ 1 };
            for(const boost::json::value& entry : entries)
            {
                if(!entry.is_object())
                {
                    continue;
                }
                boost::json::object obj = entry.as_object();
                obj["limit_characters"] = info["limit_characters"];
                m_media.push_back({ obj });
                i++;
            }
        }
        else
        {
            m_media.push_back({ info });
        }
    }

    std::optional<UrlInfo> UrlInfo::fetch(const std::string& url, const DownloaderOptions& options, const std::optional<Credential>& credential)
    {
        std::vector<std::string> arguments{ "--xff", "default", "--dump-single-json", "--skip-download", "--ignore-errors", "--flat-playlist", "--no-warnings" };
        if(options.getLimitCharacters())
        {
            arguments.push_back("--windows-filenames");
        }
        if(!options.getProxyUrl().empty())
        {
            arguments.push_back("--proxy");
            arguments.push_back(options.getProxyUrl());
        }
        if(credential)
        {
            arguments.push_back("--username");
            arguments.push_back(credential->getUsername());
            arguments.push_back("--password");
            arguments.push_back(credential->getPassword());
        }
        if(options.getCookiesBrowser() != Browser::None)
        {
            arguments.push_back("--cookies-from-browser");
            switch(options.getCookiesBrowser())
            {
            case Browser::Brave:
                arguments.push_back("brave");
                break;
            case Browser::Chrome:
                arguments.push_back("chrome");
                break;
            case Browser::Chromium:
                arguments.push_back("chromium");
                break;
            case Browser::Edge:
                arguments.push_back("edge");
                break;
            case Browser::Firefox:
                arguments.push_back("firefox");
                break;
            case Browser::Opera:
                arguments.push_back("opera");
                break;
            case Browser::Vivaldi:
                arguments.push_back("vivaldi");
                break;
            case Browser::Whale:
                arguments.push_back("whale");
                break;
            default:
                break;
            }
        }
        arguments.push_back(url);
        Process process{ Environment::findDependency("yt-dlp"), arguments };
        process.start();
        if(process.waitForExit() != 0 || process.getOutput().empty())
        {
            return std::nullopt;
        }
        boost::json::value info = boost::json::parse(process.getOutput());
        if(!info.is_object())
        {
            return std::nullopt;
        }
        boost::json::object obj = info.as_object();
        obj["limit_characters"] = options.getLimitCharacters();
        return UrlInfo{ url, obj };
    }

    const std::string& UrlInfo::getUrl() const
    {
        return m_url;
    }

    bool UrlInfo::isPlaylist() const
    {
        return m_isPlaylist;
    }

    size_t UrlInfo::count() const
    {
        return m_media.size();
    }

    Media& UrlInfo::get(const size_t index)
    {
        return m_media[index];
    }

    const Media& UrlInfo::get(const size_t index) const
    {
        return m_media[index];
    }

    Media& UrlInfo::operator[](const size_t index)
    {
        return m_media[index];
    }

    const Media& UrlInfo::operator[](const size_t index) const
    {
        return m_media[index];
    }

    std::ostream& operator<<(std::ostream& os, const UrlInfo& info)
    {
        os << "===UrlInfo===" << std::endl;
        os << "Url: " << info.m_url << std::endl;
        os << "Is Playlist: " << (info.m_isPlaylist ? "Yes" : "No") << std::endl;
        os << "Media Count: " << info.m_media.size() << std::endl;
        for(const Media& media : info.m_media)
        {
            os << media << std::endl;
        }
        return os;
    }
}