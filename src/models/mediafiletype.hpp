#pragma once

#include <optional>
#include <string>

namespace NickvisionTubeConverter::Models
{
	/**
	 * A model for media file types
	 */
    class MediaFileType
    {
    public:
    	/**
    	 * Backing enum for file types
    	 */
        enum Value
        {
            MP4 = 0,
            WEBM,
            MP3,
            OPUS,
            FLAC,
            WAV
        };

        MediaFileType() = delete;
        /**
         * Constructs a MediaFileType
         *
         * @param fileType The media file type
         */
        MediaFileType(Value fileType);
        /**
         * Attempts to parse a string to a MediaFileType
         *
         * @param s The string to parse
         * @returns A MediaFileType if valid else a std::nullopt
         */
        static std::optional<MediaFileType> parse(const std::string& s);
        operator Value() const;
        explicit operator bool() = delete;
        /**
         * Compares two MediaFileType based on == equality
         *
         * @returns True if equal, else false
         */
        bool operator==(const MediaFileType& toCompare) const;
        /**
         * Compares two MediaFileType based on != equality
         *
         * @returns True if not equal, else false
         */
        bool operator!=(const MediaFileType& toCompare) const;
        /**
         * Compares two MediaFileType::Value based on == equality
         *
         * @returns True if equal, else false
         */
        bool operator==(MediaFileType::Value toCompare) const;
        /**
         * Compares two MediaFileTypes::Value based on != equality
         *
         * @returns True if not equal, else false
         */
        bool operator!=(MediaFileType::Value toCompare) const;
        /**
         * Gets a string representation of the MediaFileType
         *
         * @returns The string representation of the MediaFileType
         */
        std::string toString() const;
        /**
         * Gets the dot extension of the file type
         *
         * @returns The dot extension of the file type
         */
        std::string toDotExtension() const;
        /**
         * Gets whether or not the MediaFileType is an audio file type
         *
         * @returns True if audio type, else false
         */
        bool isAudio() const;
        /**
         * Gets whether or not the MediaFileType is a video file type
         *
         * @returns True if video type, else false
         */
        bool isVideo() const;

    private:
        Value m_value;
    };
}

