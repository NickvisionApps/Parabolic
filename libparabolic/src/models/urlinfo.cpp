#include "models/urlinfo.h"
#include "helpers/pythonhelpers.h"

using namespace Nickvision::Keyring;
using namespace Nickvision::TubeConverter::Helpers;
using namespace pybind11::literals;
namespace py = pybind11;

namespace Nickvision::TubeConverter::Shared::Models
{
    UrlInfo::UrlInfo(const std::string& url, const py::dict& info)
        : m_url{ url },
        m_isPlaylist{ false }
    {
        if(info.is_none())
        {
            return;
        }
        if(info.contains("entries"))
        {
            m_isPlaylist = true;
            int i{ 1 };
            for(const py::handle& entry : info["entries"])
            {
                if(entry.is_none())
                {
                    continue;
                }
                py::dict entryDict{ entry.cast<py::dict>() };
                entryDict["limit_characters"] = info["limit_characters"];
                m_media.push_back({ entryDict });
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
        py::module_ ytdlp{ py::module_::import("yt_dlp") };
        py::dict ytdlpOpt;
        ytdlpOpt["quiet"] = true;
        ytdlpOpt["merge_output_format"] = "";
        ytdlpOpt["windowsfilenames"] = options.getLimitCharacters();
        ytdlpOpt["ignoreerrors"] = true;
        ytdlpOpt["extract_flat"] = "in_playlist";
        ytdlpOpt["proxy"] = options.getProxyUrl();
        if(credential)
        {
            ytdlpOpt["username"] = credential->getUsername();
            ytdlpOpt["password"] = credential->getPassword();
        }
        py::object info{ ytdlp.attr("YoutubeDL")(ytdlpOpt).attr("extract_info")(url, "download"_a=false) };
        if(info.is_none())
        {
            return std::nullopt;
        }
        info["limit_characters"] = options.getLimitCharacters();
        return UrlInfo{ url, info.cast<py::dict>() };
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