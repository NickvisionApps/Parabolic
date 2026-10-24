#include "resolution.h"
#include <algorithm>
#include <limits>

namespace parabolic
{
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

    int VideoResolution::width() const
    {
        return m_width;
    }

    int VideoResolution::height() const
    {
        return m_height;
    }

    std::string VideoResolution::str() const
    {
        if (m_width == std::numeric_limits<int>::max() && m_height == std::numeric_limits<int>::max())
        {
            return "Best";
        }
        if (m_width == -1 && m_height == -1)
        {
            return "Worst";
        }
        return std::format("{}x{}", m_width, m_height);
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