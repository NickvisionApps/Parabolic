#ifndef MEDIA_H
#define MEDIA_H

#include <chrono>
#include <optional>
#include <ostream>
#include <string>
#include <vector>
#include <json/json.h>
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
         * @param title The title of the media
         * @param duration The duration of the media
         */
        Media(const std::string& url, const std::string& title, const std::chrono::seconds& duration);
        /**
         * @brief Constructs a Media from a json object.
         * @param info The json object to construct the Media from
         * @throw std::invalid_argument If the info is None
         */
        Media(const Json::Value& info);
        /**
         * @brief Gets the URL of the media.
         * @return The URL of the media
         */
        const std::string& getUrl() const;
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
        /**
         * @brief Gets whether the media has subtitles.
         * @return True if has subtitles, else false
         */
        bool hasSubtitles() const;
        /**
         * @brief Sets whether the media has subtitles.
         * @param hasSubtitles True if has subtitles, else false
         */
        void setHasSubtitles(bool hasSubtitles);
        /**
         * @brief Outputs the Media to an output stream.
         * @param os The output stream
         * @param media The Media
         * @return The output stream
         */
        friend std::ostream& operator<<(std::ostream& os, const Media& media);

    private:
        std::string m_url;
        std::string m_title;
        std::chrono::seconds m_duration;
        std::vector<std::string> m_audioLanguages;
        std::vector<VideoResolution> m_videoResolutions;
        std::optional<unsigned int> m_playlistPosition;
        bool m_hasSubtitles;
    };
}

#endif //MEDIA_H