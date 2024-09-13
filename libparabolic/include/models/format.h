#ifndef FORMAT_H
#define FORMAT_H

#include <optional>
#include <string>
#include <boost/json.hpp>
#include "mediatype.h"
#include "videocodec.h"
#include "videoresolution.h"

namespace Nickvision::TubeConverter::Shared::Models 
{
    /**
     * @brief A model of a yt-dlp format.
     */
    class Format
    {
    public:
        /**
         * @brief Constructs a Format.
         */
        Format();
        /**
         * @brief Constructs a Format.
         * @param format The yt-dlp format json object
         */
        Format(boost::json::object format);
        /**
         * @brief Gets whether or not the Format is valid.
         * @return True if valid, else false
         */
        bool isValid() const;
        /**
         * @brief Gets the id of the format.
         * @return The id of the format
         */
        const std::string& getId() const;
        /**
         * @brief Gets the protocol of the format.
         * @return The protocol of the format
         */
        const std::string& getProtocol() const;
        /**
         * @brief Gets the type of the format.
         * @return The type of the format
         */
        MediaType getType() const;
        /**
         * @brief Gets the audio language of the format.
         * @return The audio language of the format
         */
        const std::optional<std::string>& getAudioLanguage() const;
        /**
         * @brief Gets whether the format has audio description.
         * @return Whether the format has audio description
         */
        bool hasAudioDescription() const;
        /**
         * @brief Gets the video codec of the format.
         * @return The video codec of the format
         */
        const std::optional<VideoCodec>& getVideoCodec() const;
        /**
         * @brief Gets the video resolution of the format.
         * @return The video resolution of the format
         */
        const std::optional<VideoResolution>& getVideoResolution() const;
        /**
         * @brief Gets the audio bitrate of the format.
         * @return The audio bitrate of the format
         */
        const std::optional<double>& getAudioBitrate() const;
        /**
         * @brief Gets whether or not the Format is valid.
         * @return True if valid, else false
         */
        operator bool() const;

    private:
        std::string m_id;
        std::string m_protocol;
        MediaType m_type;
        std::optional<std::string> m_audioLanguage;
        bool m_hasAudioDescription;
        std::optional<VideoCodec> m_videoCodec;
        std::optional<VideoResolution> m_videoResolution;
        std::optional<double> m_audioBitrate;
    };
}

#endif //FORMAT_H