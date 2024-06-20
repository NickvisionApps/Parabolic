#ifndef TIMEFRAME_H
#define TIMEFRAME_H

#include <chrono>
#include <optional>

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
         */
        TimeFrame(const std::chrono::seconds& start, const std::chrono::seconds& end);
        /**
         * @brief Parses a TimeFrame from start and end time strings.
         * @param start The start time string in the format "HH:MM:SS"
         * @param end The end time string in the format "HH:MM:SS"
         * @param duration The total duration in seconds
         * @returns A TimeFrame if successful, else std::nullopt
         */
        static std::optional<TimeFrame> parse(const std::string& start, const std::string& end, const std::chrono::seconds& duration);
        /**
         * @brief Gets the start time in total seconds.
         * @returns The start time in total seconds
         */
        const std::chrono::seconds& getStart() const;
        /**
         * @brief Sets the start time in total seconds.
         * @param start The start time in total seconds
         */
        void setStart(const std::chrono::seconds& start);
        /**
         * @brief Gets the end time in total seconds.
         * @returns The end time in total seconds
         */
        const std::chrono::seconds& getEnd() const;
        /**
         * @brief Sets the end time in total seconds.
         * @param end The end time in total seconds
         */
        void setEnd(const std::chrono::seconds& end);
        /**
         * @brief Gets the duration in total seconds.
         * @returns The duration in total seconds
         */
        std::chrono::seconds getDuration() const;
        /**
         * @brief Gets a string representation of the TimeFrame.
         * @brief The string will be in the format "HH:MM:SS-HH:MM:SS".
         * @returns The string representation of the TimeFrame
         */
        std::string str() const;
        /**
         * @brief Gets a string representation of the start time.
         * @brief The string will be in the format "HH:MM:SS".
         * @returns The string representation of the start time
         */
        std::string startStr() const;
        /**
         * @brief Gets a string representation of the end time.
         * @brief The string will be in the format "HH:MM:SS".
         * @returns The string representation of the end time
         */
        std::string endStr() const;

    private:
        std::chrono::seconds m_start;
        std::chrono::seconds m_end;
    };
}

#endif //TIMEFRAME_H