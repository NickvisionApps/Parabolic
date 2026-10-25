#include "media.h"

#include <algorithm>
#include <format>
#include <limits>
#include "string_manip.h"

using namespace parabolic::translator;

namespace parabolic::media
{
    TimeFrame::TimeFrame(std::chrono::seconds start, std::chrono::seconds end)
        : m_start{ start },
        m_end{ end }
    {
    }

    std::chrono::seconds TimeFrame::start() const
    {
        return m_start;
    }

    std::chrono::seconds TimeFrame::end() const
    {
        return m_end;
    }

    std::string TimeFrame::str() const
    {
        return std::format("{:%H:%M:%S} - {:%H:%M:%S}", m_start, m_end);
    }

    VideoResolution::VideoResolution()
        : m_width{ std::numeric_limits<int>::max() },
        m_height{ std::numeric_limits<int>::max() }
    {
    }

    VideoResolution::VideoResolution(int width, int height)
        : m_width{ std::max(width, height) },
        m_height{ std::min(width, height) }
    {
    }

    VideoResolution& VideoResolution::best()
    {
        static VideoResolution best{ std::numeric_limits<int>::max(), std::numeric_limits<int>::max() };
        return best;
    }

    VideoResolution& VideoResolution::worst()
    {
        static VideoResolution worst{ -1, -1 };
        return worst;
    }

    std::optional<VideoResolution> VideoResolution::parse(const std::string& value, const Translator& translator)
    {
        if (value == "Best" || value == translator._("Best"))
        {
            return best();
        }
        if (value == "Worst" || value == translator._("Worst"))
        {
            return worst();
        }
        std::vector<std::string> parts{ string_manip::split(value, 'x') };
        if (parts.size() != 2)
        {
            return std::nullopt;
        }
        try
        {
            return VideoResolution{ std::stoi(parts[0]), std::stoi(parts[1]) };
        }
        catch (...)
        {
            return std::nullopt;
        }
    }

    int VideoResolution::width() const
    {
        return m_width;
    }

    int VideoResolution::height() const
    {
        return m_height;
    }

    std::string VideoResolution::str(const Translator& translator) const
    {
        if (m_width == std::numeric_limits<int>::max() && m_height == std::numeric_limits<int>::max())
        {
            return translator._("Best");
        }
        if (m_width == -1 && m_height == -1)
        {
            return translator._("Worst");
        }
        return translator._p("Resolution", "{}x{}", m_width, m_height);
    }

    std::strong_ordering VideoResolution::operator<=>(const VideoResolution& rhs) const
    {
        if (m_width == rhs.m_width)
        {
            return m_height <=> rhs.m_height;
        }
        return m_width <=> rhs.m_width;
    }

    bool VideoResolution::operator==(const VideoResolution& rhs) const
    {
        return m_width == rhs.m_width && m_height == rhs.m_height;
    }
}