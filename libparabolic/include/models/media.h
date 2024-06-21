#ifndef MEDIA_H
#define MEDIA_H

#include <chrono>
#include <optional>
#include <string>
#include <vector>
#include "mediatype.h"
#include "videoresolution.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief A model of a downloadble media.
     */
    class Media
    {
    public:
        /**
         * @brief Constructs a Media.
         * @param url The URL of the media
         * @param type The type of the media
         * @param title The title of the media
         * @param duration The duration of the media
         */
        Media(const std::string& url, MediaType type, const std::string& title, const std::chrono::seconds& duration);
        /**
         * @brief Gets the URL of the media.
         * @return The URL of the media
         */
        const std::string& getUrl() const;
        /**
         * @brief Gets the type of the media.
         * @return The type of the media
         */
        MediaType getType() const;
        /**
         * @brief Gets the title of the media.
         * @return The title of the media
         */
        const std::string& getTitle() const;
        /**
         * @brief Gets the duration of the media.
         * @return The duration of the media
         */
        const std::chrono::seconds& getDuration() const;
        /**
         * @brief Gets the audio languages of the media.
         * @return The audio languages of the media
         */
        const std::vector<std::string>& getAudioLanguages() const;
        /**
         * @brief Adds an audio language to the media.
         * @param language The audio language to add
         */
        void addAudioLanguage(const std::string& language);
        /**
         * @brief Gets the video resolutions of the media.
         * @return The video resolutions of the media
         */
        const std::vector<VideoResolution>& getVideoResolutions() const;
        /**
         * @brief Adds a video resolution to the media.
         * @param resolution The video resolution to add
         */
        void addVideoResolution(const VideoResolution& resolution);
        /**
         * @brief Gets the playlist position of the media.
         * @return The playlist position of the media
         */
        const std::optional<unsigned int>& getPlaylistPosition() const;
        /**
         * @brief Sets the playlist position of the media.
         * @param position The playlist position to set
         */
        void setPlaylistPosition(const std::optional<unsigned int>& position);

    private:
        std::string m_url;
        MediaType m_type;
        std::string m_title;
        std::chrono::seconds m_duration;
        std::vector<std::string> m_audioLanguages;
        std::vector<VideoResolution> m_videoResolutions;
        std::optional<unsigned int> m_playlistPosition;
    };
}

#endif //MEDIA_H