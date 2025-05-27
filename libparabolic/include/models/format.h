#ifndef FORMAT_H
#define FORMAT_H

#include <optional>
#include <string>
#include <boost/json.hpp>
#include "audiocodec.h"
#include "formatvalue.h"
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
         * @param value The predetermined format value
         * @param type The type of media the format applies to
         */
        Format(FormatValue value, MediaType type);
        /**
         * @brief Constructs a Format.
         * @param json The JSON object to construct the Format from
         * @param isYtdlpJson Whether or not the json object is in yt-dlp json format
         */
        Format(boost::json::object json, bool isYtdlpJson = true);
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
         * @brief Gets the extension of the format.
         * @return The extension of the format
         */
        const std::string& getExtension() const;
        /**
         * @brief Gets the type of the format.
         * @return The type of the format
         */
        MediaType getType() const;
        /**
         * @brief Gets the bitrate of the format in kbps.
         * @return The bitrate bitrate of the format in kbps
         */
        const std::optional<double>& getBitrate() const;
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
         * @brief Gets the audio codec of the format.
         * @return The audio codec of the format
         */
        const std::optional<AudioCodec>& getAudioCodec() const;
        /**
         * @brief Gets the video resolution of the format.
         * @return The video resolution of the format
         */
        const std::optional<VideoResolution>& getVideoResolution() const;
        /**
         * @brief Gets whether or not this format represents a format value.
         * @param value The format value to test
         * @return True if this format represents the format value
         * @return False if this format does not represent the format value
         */
        bool isFormatValue(FormatValue value) const;
        /**
         * @brief Gets the string representation of the format.
         * @return The string representation of the format
         */
        std::string str() const;
        /**
         * @brief Converts the Format to a JSON object.
         * @brief The json object will not be in yt-dlp json format.
         * @return The JSON object
         */
        boost::json::object toJson() const;
        /**
         * @brief Compares two Format via ==.
         * @param other The other Format to compare
         * @return True if this == other
         */
        bool operator==(const Format& other) const;
        /**
         * @brief Compares two Format via !=.
         * @param other The other Format to compare
         * @return True if this != other
         */
        bool operator!=(const Format& other) const;
        /**
         * @brief Compares two Format via <.
         * @param other The other Format to compare
         * @return True if this < other
         */
        bool operator<(const Format& other) const;
        /**
         * @brief Compares two Format via >.
         * @param other The other Format to compare
         * @return True if this > other
         */
        bool operator>(const Format& other) const;

    private:
        std::string m_id;
        std::string m_protocol;
        std::string m_extension;
        unsigned long long m_bytes;
        MediaType m_type;
        std::optional<double> m_bitrate;
        std::optional<std::string> m_audioLanguage;
        bool m_hasAudioDescription;
        std::optional<VideoCodec> m_videoCodec;
        std::optional<AudioCodec> m_audioCodec;
        std::optional<VideoResolution> m_videoResolution;
    };
}

#endif //FORMAT_H
