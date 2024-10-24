#include "models/timeframe.h"
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <libnick/helpers/stringhelpers.h>

using namespace Nickvision::Helpers;

namespace Nickvision::TubeConverter::Shared::Models
{
    static std::string secondsStr(const std::chrono::seconds& totalSeconds)
    {
        long long hours{ totalSeconds.count() / 3600 };
        long long minutes{ (totalSeconds.count() % 3600) / 60 };
        long long seconds{ (totalSeconds.count() % 3600) % 60 };
        std::stringstream builder;
        builder << std::setfill('0') << std::setw(2) << hours;
        builder << ":" << std::setfill('0') << std::setw(2) << minutes;
        builder << ":" << std::setfill('0') << std::setw(2) << seconds;
        return builder.str();
    }

    TimeFrame::TimeFrame(const std::chrono::seconds& start, const std::chrono::seconds& end)
        : m_start{ start }, 
        m_end{ end }
    {
        if(m_end < m_start)
        {
            throw std::invalid_argument("The end time must be after the start time");
        }
    }

    TimeFrame::TimeFrame(boost::json::object json)
        : m_start{ json["Start"].is_int64() ? std::chrono::seconds(json["Start"].as_int64()) : std::chrono::seconds(0) },
        m_end{ json["End"].is_int64() ? std::chrono::seconds(json["End"].as_int64()) : std::chrono::seconds(0) }
    {

    }

    std::optional<TimeFrame> TimeFrame::parse(const std::string& start, const std::string& end, const std::chrono::seconds& duration)
    {
        if(start.empty() || end.empty() || duration.count() == 0)
        {
            return std::nullopt;
        }
        std::vector<std::string> startParts{ StringHelpers::split(start, ":") };
        std::vector<std::string> endParts{ StringHelpers::split(end, ":") };
        if(startParts.size() != 3 || endParts.size() != 3)
        {
            return std::nullopt;
        }
        std::chrono::seconds startSeconds;
        std::chrono::seconds endSeconds;
        try
        {
            startSeconds = std::chrono::seconds{ std::stoi(startParts[0]) * 3600 + std::stoi(startParts[1]) * 60 + std::stoi(startParts[2]) };
            endSeconds = std::chrono::seconds{ std::stoi(endParts[0]) * 3600 + std::stoi(endParts[1]) * 60 + std::stoi(endParts[2]) };
        }
        catch(...)
        {
            return std::nullopt;
        }
        if(startSeconds < std::chrono::seconds(0) || endSeconds <= startSeconds || endSeconds > duration)
        {
            return std::nullopt;
        }
        return TimeFrame{ startSeconds, endSeconds };
    }

    const std::chrono::seconds& TimeFrame::getStart() const
    {
        return m_start;
    }

    void TimeFrame::setStart(const std::chrono::seconds& start)
    {
        m_start = start;
    }

    const std::chrono::seconds& TimeFrame::getEnd() const
    {
        return m_end;
    }

    void TimeFrame::setEnd(const std::chrono::seconds& end)
    {
        m_end = end;
    }

    std::chrono::seconds TimeFrame::getDuration() const
    {
        return m_end - m_start;
    }

    std::string TimeFrame::str() const
    {
        return secondsStr(m_start) + "-" + secondsStr(m_end);
    }

    std::string TimeFrame::startStr() const
    {
        return secondsStr(m_start);
    }

    std::string TimeFrame::endStr() const
    {
        return secondsStr(m_end);
    }

    boost::json::object TimeFrame::toJson() const
    {
        boost::json::object json;
        json["Start"] = m_start.count();
        json["End"] = m_end.count();
        return json;
    }

    bool TimeFrame::operator==(const TimeFrame& other) const
    {
        if(m_start == other.m_start && m_end == other.m_end)
        {
            return true;
        }
        return (m_end - m_start) - (other.m_end - other.m_start) < std::chrono::seconds(1);
    }

    bool TimeFrame::operator!=(const TimeFrame& other) const
    {
        return !operator==(other);
    }
}