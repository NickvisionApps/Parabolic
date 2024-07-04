#include "models/media.h"
#include <algorithm>
#include <stdexcept>
#include <libnick/helpers/stringhelpers.h>

using namespace Nickvision::Helpers;
namespace py = pybind11;

namespace Nickvision::TubeConverter::Shared::Models
{
    Media::Media(const std::string& url, const std::string& title, const std::chrono::seconds& duration)
        : m_url{ url },
        m_title{ title },
        m_duration{ duration },
        m_playlistPosition{ std::nullopt },
        m_hasSubtitles{ false }
    {

    }

    Media::Media(const py::dict& info)
        : m_duration{ 0 },
        m_playlistPosition{ std::nullopt },
        m_hasSubtitles{ false }
    {
        if(info.is_none())
        {
            throw std::invalid_argument("The info dictionary is None");
        }
        //Parse base information
        m_url = info.contains("url") ? info["url"].cast<std::string>() : info["webpage_url"].cast<std::string>();
        m_title = info.contains("title") ? info["title"].cast<std::string>() : "Media";
        m_title = StringHelpers::normalizeForFilename(m_title, info["limit_characters"].cast<bool>());
        if(info.contains("duration"))
        {
            m_duration = std::chrono::seconds{ static_cast<int>(info["duration"].cast<double>()) };
        }
        if(info.contains("language") && !info["language"].is_none())
        {
            m_audioLanguages.push_back(info["language"].cast<std::string>());
        }
        if(info.contains("width") && info.contains("height"))
        {
            m_videoResolutions.push_back({ info["width"].cast<int>(), info["height"].cast<int>() });
        }
        else if(info.contains("resolution") && info["resolution"].cast<std::string>() != "audio only")
        {
            std::optional<VideoResolution> res{ VideoResolution::parse(info["resolution"].cast<std::string>()) };
            if(res)
            {
                m_videoResolutions.push_back(*res);
            }
        }
        if(info.contains("subtitles"))
        {
            py::dict subtitles{ info["subtitles"].cast<py::dict>() };
            if(!subtitles.empty())
            {
                m_hasSubtitles = !(subtitles.size() == 1 && subtitles.contains("live_chat"));
            }
        }
        //Parse formats
        if(info.contains("formats"))
        {
            for(const py::handle& entry : info["formats"])
            {
                if(entry.is_none())
                {
                    continue;
                }
                py::dict format{ entry.cast<py::dict>() };
                if(format.contains("language") && !format["language"].is_none())
                {
                    std::string language{ format["language"].cast<std::string>() };
                    if(std::find(m_audioLanguages.begin(), m_audioLanguages.end(), language) == m_audioLanguages.end())
                    {
                        m_audioLanguages.push_back(language);
                    }
                }
                if(format.contains("vcodec") && format["vcodec"].cast<std::string>() != "none")
                {
                    if(format.contains("resolution") && !format["resolution"].is_none() && format["resolution"].cast<std::string>() != "audio only")
                    {
                        std::optional<VideoResolution> res{ VideoResolution::parse(format["resolution"].cast<std::string>()) };
                        if(res && std::find(m_videoResolutions.begin(), m_videoResolutions.end(), *res) == m_videoResolutions.end())
                        {
                            m_videoResolutions.push_back(*res);
                        }
                    }
                }
            }
        }
        //Sort
        std::sort(m_audioLanguages.begin(), m_audioLanguages.end());
        std::sort(m_videoResolutions.begin(), m_videoResolutions.end(), std::greater{});
        if(info.contains("video_ext") && info["video_ext"].cast<std::string>() != "none")
        {
            m_videoResolutions.insert(m_videoResolutions.begin(), *VideoResolution::parse("Best"));
        }
    }

    const std::string& Media::getUrl() const
    {
        return m_url;
    }

    const std::string& Media::getTitle() const
    {
        return m_title;
    }

    const std::chrono::seconds& Media::getDuration() const
    {
        return m_duration;
    }

    const std::vector<std::string>& Media::getAudioLanguages() const
    {
        return m_audioLanguages;
    }

    void Media::addAudioLanguage(const std::string& language)
    {
        m_audioLanguages.push_back(language);
    }

    const std::vector<VideoResolution>& Media::getVideoResolutions() const
    {
        return m_videoResolutions;
    }

    void Media::addVideoResolution(const VideoResolution& resolution)
    {
        m_videoResolutions.push_back(resolution);
    }

    const std::optional<unsigned int>& Media::getPlaylistPosition() const
    {
        return m_playlistPosition;
    }

    void Media::setPlaylistPosition(const std::optional<unsigned int>& position)
    {
        m_playlistPosition = position;
    }

    bool Media::hasSubtitles() const
    {
        return m_hasSubtitles;
    }

    void Media::setHasSubtitles(bool hasSubtitles)
    {
        m_hasSubtitles = hasSubtitles;
    }

    std::ostream& operator<<(std::ostream& os, const Media& media)
    {
        os << "===Media===" << std::endl;
        os << "URL: " << media.m_url << std::endl;
        os << "Title: " << media.m_title << std::endl;
        os << "Duration: " << media.m_duration.count() << " seconds" << std::endl;
        os << "Audio Languages: ";
        for(const std::string& language : media.m_audioLanguages)
        {
            os << language << ", ";
        }
        os << std::endl;
        os << "Video Resolutions: ";
        for(const VideoResolution& resolution : media.m_videoResolutions)
        {
            os << resolution << ", ";
        }
        os << std::endl;
        os << "Playlist Position: " << media.m_playlistPosition.value_or(0) << std::endl;
        os << "Has Subtitles: " << (media.m_hasSubtitles ? "Yes" : "No");
        return os;
    }
}