#include "models/media.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    Media::Media(const std::string& url, MediaType type, const std::string& title, const std::chrono::seconds& duration)
        : m_url{ url },
        m_type{ type },
        m_title{ title },
        m_duration{ duration }
    {

    }

    const std::string& Media::getUrl() const
    {
        return m_url;
    }

    MediaType Media::getType() const
    {
        return m_type;
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
}