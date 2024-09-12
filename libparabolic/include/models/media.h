#ifndef MEDIA_H
#define MEDIA_H

#include <chrono>
#include <optional>
#include <ostream>
#include <string>
#include <vector>
#include <boost/json.hpp>
#include "format.h"
#include "mediatype.h"
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
         * @brief Gets whether the media has subtitles.
         * @return True if has subtitles, else false
         */
        bool hasSubtitles() const;
        /**
         * @brief Gets the formats of the media.
         * @return The formats of the media
         */
        const std::vector<Format>& getFormats() const;
        /**
         * @brief Gets the automatic subtitles of the media.
         * @return The automatic subtitles of the media
         */
        const std::vector<std::string>& getAutomaticSubtitles() const;
        /**
         * @brief Gets the subtitles of the media.
         * @return The subtitles of the media
         */
        const std::vector<std::string>& getSubtitles() const;

    private:
        std::string m_url;
        std::string m_title;
        MediaType m_type;
        TimeFrame m_timeFrame;
        bool m_hasSubtitles;
        std::vector<Format> m_formats;
        std::vector<std::string> m_automaticSubtitles;
        std::vector<std::string> m_subtitles;
    };
}

#endif //MEDIA_H