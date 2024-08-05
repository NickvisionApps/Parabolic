#include "models/urlinfo.h"
#include <libnick/system/environment.h>
#include <libnick/system/process.h>

using namespace Nickvision::Keyring;
using namespace Nickvision::System;

namespace Nickvision::TubeConverter::Shared::Models
{
    UrlInfo::UrlInfo(const std::string& url, const Json::Value& info)
        : m_url{ url },
        m_isPlaylist{ false }
    {
        Json::Value entries{ info.get("entries", Json::Value()) };
        if(!entries.isNull() && !entries.empty())
        {
            m_isPlaylist = true;
            int i{ 1 };
            for(Json::Value entry : entries)
            {
                if(entry.isNull() || entry.empty())
                {
                    continue;
                }
                entry["limit_characters"] = info["limit_characters"];
                Media media{ entry };
                media.setPlaylistPosition(i);
                m_media.push_back(media);
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
        std::vector<std::string> args{ "--xff", "default", "--dump-single-json", "--skip-download", "--ignore-errors", "--flat-playlist", "--no-warnings" };
        if(options.getLimitCharacters())
        {
            args.push_back("--windows-filenames");
        }
        if(!options.getProxyUrl().empty())
        {
            args.push_back("--proxy");
            args.push_back(options.getProxyUrl());
        }
        if(credential)
        {
            args.push_back("--username");
            args.push_back(credential->getUsername());
            args.push_back("--password");
            args.push_back(credential->getPassword());
        }
        args.push_back(url);
        Process process{ Environment::findDependency("yt-dlp"), args };
        process.start();
        if(process.waitForExit() != 0 || process.getOutput().empty())
        {
            return std::nullopt;
        }
        Json::Value info;
        Json::Reader reader;
        if(!reader.parse(process.getOutput(), info))
        {
            return std::nullopt;
        }
        info["limit_characters"] = options.getLimitCharacters();
        return UrlInfo{ url, info };
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