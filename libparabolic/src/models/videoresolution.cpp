#include "models/videoresolution.h"
#include <limits>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/gettext.h>

using namespace Nickvision::Helpers;

namespace Nickvision::TubeConverter::Shared::Models
{
    VideoResolution::VideoResolution()
        : m_width{ std::numeric_limits<int>::max() },
        m_height{ std::numeric_limits<int>::max() }
    {

    }

    VideoResolution::VideoResolution(int width, int height)
        : m_width{ width },
        m_height{ height }
    {

    }

    VideoResolution::VideoResolution(boost::json::object json)
        : m_width{ json["Width"].is_int64() ? static_cast<int>(json["Width"].as_int64()) : std::numeric_limits<int>::max() },
        m_height{ json["Height"].is_int64() ? static_cast<int>(json["Height"].as_int64()) : std::numeric_limits<int>::max() }
    {

    }

    std::optional<VideoResolution> VideoResolution::parse(const std::string& value)
    {
        if(value == "Best" || value == _("Best"))
        {
            return VideoResolution{};
        }
        const std::vector<std::string> parts{ StringHelpers::split(value, "x") };
        if(parts.size() == 2)
        {
            try
            {
                int width{ std::stoi(parts[0]) };
                int height{ std::stoi(parts[1]) };
                return VideoResolution{ width, height };
            }
            catch(...) { }
        }
        return std::nullopt;
    }

    bool VideoResolution::isValid() const
    {
        return m_width > 0 && m_height > 0;
    }

    bool VideoResolution::isBest() const
    {
        return m_width == std::numeric_limits<int>::max() && m_height == std::numeric_limits<int>::max();
    }

    int VideoResolution::getWidth() const
    {
        return m_width;
    }

    int VideoResolution::getHeight() const
    {
        return m_height;
    }

    std::string VideoResolution::str() const
    {
        if(isBest())
        {
            return _("Best");
        }
        return std::to_string(m_width) + "x" + std::to_string(m_height);
    }

    boost::json::object VideoResolution::toJson() const
    {
        boost::json::object json;
        json["Width"] = m_width;
        json["Height"] = m_height;
        return json;
    }

    bool VideoResolution::operator==(const VideoResolution& other) const
    {
        return m_width == other.m_width && m_height == other.m_height;
    }

    bool VideoResolution::operator!=(const VideoResolution& other) const
    {
        return !operator==(other);
    }

    bool VideoResolution::operator<(const VideoResolution& other) const
    {
        return m_width < other.m_width || (m_width == other.m_width && m_height < other.m_height);
    }

    bool VideoResolution::operator>(const VideoResolution& other) const
    {
        return m_width > other.m_width || (m_width == other.m_width && m_height > other.m_height);
    }

    VideoResolution::operator bool() const
    {
        return isValid();
    }
}