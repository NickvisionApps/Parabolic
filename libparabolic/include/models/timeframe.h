#ifndef TIMEFRAME_H
#define TIMEFRAME_H

#include <chrono>
#include <optional>
#include <string>
#include <boost/json.hpp>

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief A model of a time frame.
     */
    class TimeFrame
    {
    public:
        /**
         * @brief Constructs a TimeFrame.
         * @param start The start time in total seconds
         * @param end The end time in total seconds
         * @throw std::invalid_argument Thrown if the end time is before the start time
         */
        TimeFrame(const std::chrono::seconds& start, const std::chrono::seconds& end);
        /**
         * @brief Constructs a TimeFrame.
         * @param json The JSON object to construct the TimeFrame from
         */
        TimeFrame(boost::json::object json);
        /**
         * @brief Parses a TimeFrame from start and end time strings.
         * @param start The start time string in the format "HH:MM:SS"
         * @param end The end time string in the format "HH:MM:SS"
         * @param duration The total duration in seconds
         * @return A TimeFrame if successful, else std::nullopt
         */
        static std::optional<TimeFrame> parse(const std::string& start, const std::string& end, const std::chrono::seconds& duration);
        /**
         * @brief Gets the start time in total seconds.
         * @return The start time in total seconds
         */
        const std::chrono::seconds& getStart() const;
        /**
         * @brief Sets the start time in total seconds.
         * @param start The start time in total seconds
         */
        void setStart(const std::chrono::seconds& start);
        /**
         * @brief Gets the end time in total seconds.
         * @return The end time in total seconds
         */
        const std::chrono::seconds& getEnd() const;
        /**
         * @brief Sets the end time in total seconds.
         * @param end The end time in total seconds
         */
        void setEnd(const std::chrono::seconds& end);
        /**
         * @brief Gets the duration in total seconds.
         * @return The duration in total seconds
         */
        std::chrono::seconds getDuration() const;
        /**
         * @brief Gets a string representation of the TimeFrame.
         * @brief The string will be in the format "HH:MM:SS-HH:MM:SS".
         * @return The string representation of the TimeFrame
         */
        std::string str() const;
        /**
         * @brief Gets a string representation of the start time.
         * @brief The string will be in the format "HH:MM:SS".
         * @return The string representation of the start time
         */
        std::string startStr() const;
        /**
         * @brief Gets a string representation of the end time.
         * @brief The string will be in the format "HH:MM:SS".
         * @return The string representation of the end time
         */
        std::string endStr() const;
        /**
         * @brief Converts the TimeFrame to a JSON object.
         * @return The JSON object representation of the TimeFrame
         */
        boost::json::object toJson() const;
        /**
         * @brief Compares two TimeFrames via ==.
         * @param other The other TimeFrame to compare
         * @return True if this == other, false otherwise
         */
        bool operator==(const TimeFrame& other) const;
        /**
         * @brief Compares two TimeFrames via !=.
         * @param other The other TimeFrame to compare
         * @return True if this != other, false otherwise
         */
        bool operator!=(const TimeFrame& other) const;

    private:
        std::chrono::seconds m_start;
        std::chrono::seconds m_end;
    };
}

#endif //TIMEFRAME_H