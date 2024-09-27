#ifndef VIDEORESOLUTION_H
#define VIDEORESOLUTION_H

#include <optional>
#include <ostream>
#include <string>

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief A model of a video resolution.
     */
    class VideoResolution
    {
    public:
        /**
         * @brief Constructs a VideoResolution.
         * @brief This will be the "best" resolution.
         */
        VideoResolution();
        /**
         * @brief Constructs a VideoResolution.
         * @param width The width of the resolution
         * @param height The height of the resolution
         */
        VideoResolution(int width, int height);
        /**
         * @brief Parses a VideoResolution from a string.
         * @param value The string to parse (Format: "WidthxHeight")
         * @return The parsed VideoResolution or std::nullopt if the string is invalid
         */
        static std::optional<VideoResolution> parse(const std::string& value);
        /**
         * @brief Gets whether or not the object is valid.
         * @return True if valid, else false
         */
        bool isValid() const;
        /**
         * @brief Gets whether or not the resolution is the best.
         * @return True if the resolution is the best, else false
         */
        bool isBest() const;
        /**
         * @brief Gets the width of the resolution.
         * @return The width of the resolution
         */
        int getWidth() const;
        /**
         * @brief Gets the height of the resolution.
         * @return The height of the resolution
         */
        int getHeight() const;
        /**
         * @brief Gets the string representation of the resolution.
         * @return The string representation of the resolution
         */
        std::string str() const;
        /**
         * @brief Compares two VideoResolutions via ==.
         * @param other The other VideoResolution to compare
         * @return True if this == other
         */
        bool operator==(const VideoResolution& other) const;
        /**
         * @brief Compares two VideoResolutions via !=.
         * @param other The other VideoResolution to compare
         * @return True if this != other
         */
        bool operator!=(const VideoResolution& other) const;
        /**
         * @brief Compares two VideoResolutions via <.
         * @param other The other VideoResolution to compare
         * @return True if this < other
         */
        bool operator<(const VideoResolution& other) const;
        /**
         * @brief Compares two VideoResolutions via >.
         * @param other The other VideoResolution to compare
         * @return True if this > other
         */
        bool operator>(const VideoResolution& other) const;
        /**
         * @brief Gets whether or not the object is valid.
         * @return True if valid, else false
         */
        operator bool() const;

    private:
        int m_width;
        int m_height;
    };
}

#endif //VIDEORESOLUTION_H