#include "models/media.h"
#include <algorithm>
#include <stdexcept>
#include <libnick/helpers/stringhelpers.h>

using namespace Nickvision::Helpers;

namespace Nickvision::TubeConverter::Shared::Models
{
    Media::Media(const std::string& url, const std::string& title, const std::chrono::seconds& duration, MediaType type)
        : m_url{ url },
        m_title{ title },
        m_timeFrame{ std::chrono::seconds(0), duration },
        m_type{ type },
        m_playlistPosition{ std::nullopt },
        m_hasSubtitles{ false }
    {

    }

    Media::Media(boost::json::object info)
        : m_timeFrame{ std::chrono::seconds(0), std::chrono::seconds(0) },
        m_playlistPosition{ std::nullopt },
        m_hasSubtitles{ false }
    {
        if(info.empty())
        {
            throw std::invalid_argument("The info object is empty");
        }
        //Parse base information
        m_url = info.contains("url") ? (info["url"].is_string() ? info["url"].as_string() : "") : (info["webpage_url"].is_string() ? info["webpage_url"].as_string().c_str() : "");
        m_title = info["title"].is_string() ? info["title"].as_string() : "Media";
        m_title = StringHelpers::normalizeForFilename(m_title, info["limit_characters"].is_bool() ? info["limit_characters"].as_bool() : false);
        if(info.contains("duration"))
        {
            m_timeFrame = { std::chrono::seconds(0), std::chrono::seconds{ info["duration"].is_double() ? static_cast<int>(info["duration"].as_double()) : (info["duration"].is_int64() ? static_cast<int>(info["duration"].as_int64()) : 0) } };
        }
        if(info.contains("language"))
        {
            std::string language{ info["language"].is_string() ? info["language"].as_string() : "" };
            if(!language.empty() && language != "none")
            {
                m_audioLanguages.push_back(language);
            }
        }
        if(info.contains("width") && info.contains("height"))
        {
            m_videoResolutions.push_back({ info["width"].is_int64() ? static_cast<int>(info["width"].as_int64()) : 0, info["height"].is_int64() ? static_cast<int>(info["height"].as_int64()) : 0 });
        }
        else if(info.contains("resolution"))
        {
            std::string resolution{ info["resolution"].is_string() ? info["resolution"].as_string() : "" };
            if(!resolution.empty() && resolution != "audio only")
            {
                std::optional<VideoResolution> res{ VideoResolution::parse(resolution) };
                if(res)
                {
                    m_videoResolutions.push_back(*res);
                }
            }
        }
        if(info.contains("subtitles"))
        {
            boost::json::object subtitles{ info["subtitles"].is_object() ? info["subtitles"].as_object() : boost::json::object() };
            if(!subtitles.empty())
            {
                m_hasSubtitles = !(subtitles.size() == 1 && subtitles.contains("live_chat"));
            }
        }
        //Parse formats
        if(info.contains("formats") && info["formats"].is_array())
        {
            for(const boost::json::value& format : info["formats"].as_array())
            {
                if(!format.is_object())
                {
                    continue;
                }
                boost::json::object obj{ format.as_object() };
                if(obj.contains("language"))
                {
                    std::string language{ obj["language"].is_string() ? obj["language"].as_string() : "" };
                    if(!language.empty() && language != "none")
                    {
                        if(std::find(m_audioLanguages.begin(), m_audioLanguages.end(), language) == m_audioLanguages.end())
                        {
                            m_audioLanguages.push_back(language);
                        }
                    }
                }
                if(obj.contains("vcodec"))
                {
                    std::string vcodec{ obj["vcodec"].is_string() ? obj["vcodec"].as_string() : "" };
                    if(!vcodec.empty() && vcodec != "none")
                    {
                        if(obj.contains("resolution"))
                        {
                            std::string resolution{ obj["resolution"].is_string() ? obj["resolution"].as_string() : "" };
                            if(!resolution.empty() && resolution != "none" && resolution != "audio only")
                            {
                                std::optional<VideoResolution> res{ VideoResolution::parse(resolution) };
                                if(res && std::find(m_videoResolutions.begin(), m_videoResolutions.end(), *res) == m_videoResolutions.end())
                                {
                                    m_videoResolutions.push_back(*res);
                                }
                            }
                        }
                    }
                }
            }
        }
        //Sort
        std::sort(m_audioLanguages.begin(), m_audioLanguages.end());
        std::sort(m_videoResolutions.begin(), m_videoResolutions.end(), std::greater{});
        if(info.contains("video_ext"))
        {
            std::string videoExt{ info["video_ext"].is_string() ? info["video_ext"].as_string() : "" };
            if(!videoExt.empty() && videoExt != "none")
            {
                m_videoResolutions.insert(m_videoResolutions.begin(), VideoResolution{});
            }
        }
        else if(!m_videoResolutions.empty())
        {
            m_videoResolutions.insert(m_videoResolutions.begin(), VideoResolution{});
        }
        m_type = m_videoResolutions.empty() ? MediaType::Audio : MediaType::Video;
    }

    const std::string& Media::getUrl() const
    {
        return m_url;
    }

    const std::string& Media::getTitle() const
    {
        return m_title;
    }

    const TimeFrame& Media::getTimeFrame() const
    {
        return m_timeFrame;
    }

    MediaType Media::getType() const
    {
        return m_type;
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
        os << "TimeFrame: " << media.m_timeFrame.str() << std::endl;
        os << "Type: " << (media.m_type == MediaType::Audio ? "Audio" : "Video") << std::endl;
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