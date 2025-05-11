#ifndef MEDIA_H
#define MEDIA_H

#include <filesystem>
#include <string>
#include <vector>
#include <boost/json.hpp>
#include "format.h"
#include "mediatype.h"
#include "subtitlelanguage.h"
#include "timeframe.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief A model of a downloadable yt-dlp media.
     */
    class Media
    {
    public:
        /**
         * @brief Constructs a Media from a yt-dlp json object.
         * @param info The json object to construct the Media from
         */
        Media(boost::json::object info);
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
         * @brief Gets the playlist position of the media.
         * @return The playlist position of the media
         * @return -1 if the media is not part of a playlist
         */
        int getPlaylistPosition() const;
        /**
         * @brief Gets the type of the media.
         * @return The type of the media
         */
        MediaType getType() const;
        /**
         * @brief Gets the time frame of the media.
         * @return The time frame of the media
         */
        const TimeFrame& getTimeFrame() const;
        /**
         * @brief Gets the formats of the media.
         * @return The formats of the media
         */
        const std::vector<Format>& getFormats() const;
        /**
         * @brief Gets the subtitles of the media.
         * @return The subtitles of the media
         */
        const std::vector<SubtitleLanguage>& getSubtitles() const;
        /**
         * @brief Gets the suggested save folder of the media.
         * @return The suggested save folder of the media
         */
        const std::filesystem::path& getSuggestedSaveFolder() const;

    private:
        std::string m_url;
        std::string m_title;
        int m_playlistPosition;
        MediaType m_type;
        TimeFrame m_timeFrame;
        std::vector<Format> m_formats;
        std::vector<SubtitleLanguage> m_subtitles;
        std::filesystem::path m_suggestedSaveFolder;
    };
}

#endif //MEDIA_H
