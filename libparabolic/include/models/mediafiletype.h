#ifndef MEDIAFILETYPE_H
#define MEDIAFILETYPE_H

#include <optional>
#include <string>

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief A model of a media file type.
     */
    class MediaFileType
    {
    public:
        /**
         * @brief Supported media file types.
         */
        enum MediaFileTypeValue
        {
            Video,
            MP4,
            WEBM,
            MKV,
            MOV,
            AVI,
            Audio,
            MP3,
            M4A,
            OPUS,
            FLAC,
            WAV
        };
        /**
         * @brief Constructs a MediaFileType.
         * @param value The media file type
         */
        MediaFileType(MediaFileTypeValue value);
        /**
         * @brief Parses a media file type from a string.
         * @param value The string to parse
         * @return The media file type if successful, else std::nullopt
         */
        static std::optional<MediaFileType> parse(std::string value);
        /**
         * @brief Gets the number of supported video file types.
         * @return The number of video file types
         */
        static int getVideoFileTypeCount();
        /**
         * @brief Gets the number of supported audio file types.
         * @return The number of audio file types
         */
        static int getAudioFileTypeCount();
        /**
         * @brief Gets the file extension (with the .) of the media file type.
         * @return The dot extension of the media file type
         */
        std::string getDotExtension() const;
        /**
         * @brief Gets whether or not the file type is an audio file type.
         * @return True if audio file type, else false
         */
        bool isAudio() const;
        /**
         * @brief Gets whether or not the file type is a video file type.
         * @return True if video file type, else false
         */
        bool isVideo() const;
        /**
         * @brief Gets whether or not the file type is a generic file type.
         * @return True if generic file type, else false
         */
        bool isGeneric() const;
        /**
         * @brief Gets whether or not the file type supports embedding thumbnails.
         * @return True if supports embedding thumbnails, else false
         */
        bool supportsThumbnails() const;
        /**
         * @brief Gets whether or not the file type supports embedding subtitles.
         * @return True if supports embedding subtitles, else false
         */
        bool supportsSubtitles() const;
        /**
         * @brief Gets the string representation of the media file type.
         * @return The string representation of the media file type
         */
        std::string str() const;
        /**
         * @brief Gets the media file type value.
         * @return The media file type value
         */
        operator MediaFileTypeValue() const;
        /**
         * @brief Gets the media file type value as an int.
         * @return The media file type value as an int
         */
        operator int() const;
        /**
         * @brief Gets the media file type value as an unsigned int.
         * @return The media file type value as an unsigned int
         */
        operator unsigned int() const;
        /**
         * @brief Gets the media file type value as a size_t.
         * @return The media file type value as an size_t
         */
        operator size_t() const;
        /**
         * @brief Compares two MediaFileTypes via ==.
         * @param other The other MediaFileType
         * @return True if this == other, else false
         */
        bool operator==(const MediaFileType& other) const;
        /**
         * @brief Compares two MediaFileTypes via !=.
         * @param other The other MediaFileType
         * @return True if this != other, else false
         */
        bool operator!=(const MediaFileType& other) const;
        /**
         * @brief Compares a MediaFileType to a MediaFileTypeValue via ==.
         * @param value The MediaFileTypeValue
         * @return True if this == value, else false
         */
        bool operator==(MediaFileTypeValue value) const;
        /**
         * @brief Compares a MediaFileType to a MediaFileTypeValue via !=.
         * @param value The MediaFileTypeValue
         * @return True if this != value, else false
         */
        bool operator!=(MediaFileTypeValue value) const;
        operator bool() const = delete;

    private:
        MediaFileTypeValue m_value;
    };
}

#endif //MEDIAFILETYPE_H
