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

    Media::Media(const Json::Value& info)
        : m_timeFrame{ std::chrono::seconds(0), std::chrono::seconds(0) },
        m_playlistPosition{ std::nullopt },
        m_hasSubtitles{ false }
    {
        if(info.isNull() || info.empty())
        {
            throw std::invalid_argument("The info object is empty");
        }
        //Parse base information
        m_url = info.isMember("url") ? info.get("url", "").asString() : info.get("webpage_url", "").asString();
        m_title = info.get("title", "Media").asString();
        m_title = StringHelpers::normalizeForFilename(m_title, info.get("limit_characters", false).asBool());
        if(info.isMember("duration"))
        {
            m_timeFrame = { std::chrono::seconds(0), std::chrono::seconds{ static_cast<int>(info.get("duration", 0.0).asDouble()) } };
        }
        if(info.isMember("language"))
        {
            std::string language{ info.get("language", "").asString() };
            if(!language.empty() && language != "none")
            {
                m_audioLanguages.push_back(language);
            }
        }
        if(info.isMember("width") && info.isMember("height"))
        {
            m_videoResolutions.push_back({ info.get("width", 0).asInt(), info.get("height", 0).asInt() });
        }
        else if(info.isMember("resolution"))
        {
            std::string resolution{ info.get("resolution", "").asString() };
            if(!resolution.empty() && resolution != "audio only")
            {
                std::optional<VideoResolution> res{ VideoResolution::parse(resolution) };
                if(res)
                {
                    m_videoResolutions.push_back(*res);
                }
            }
        }
        if(info.isMember("subtitles"))
        {
            Json::Value subtitles{ info.get("subtitles", Json::Value()) };
            if(!subtitles.isNull() && !subtitles.empty())
            {
                m_hasSubtitles = !(subtitles.size() == 1 && subtitles.isMember("live_chat"));
            }
        }
        //Parse formats
        if(info.isMember("formats"))
        {
            for(const Json::Value& format : info["formats"])
            {
                if(format.isNull() || format.empty())
                {
                    continue;
                }
                if(format.isMember("language"))
                {
                    std::string language{ format.get("language", "").asString() };
                    if(!language.empty() && language != "none")
                    {
                        if(std::find(m_audioLanguages.begin(), m_audioLanguages.end(), language) == m_audioLanguages.end())
                        {
                            m_audioLanguages.push_back(language);
                        }
                    }
                }
                if(format.isMember("vcodec"))
                {
                    std::string vcodec{ format.get("vcodec", "").asString() };
                    if(!vcodec.empty() && vcodec != "none")
                    {
                        if(format.isMember("resolution"))
                        {
                            std::string resolution{ format.get("resolution", "").asString() };
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
        if(info.isMember("video_ext"))
        {
            std::string videoExt{ info.get("video_ext", "").asString() };
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