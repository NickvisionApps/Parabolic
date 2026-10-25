#include "media.h"

#include <algorithm>
#include <chrono>
#include <compare>
#include <format>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "string_manip.h"
#include "translator.h"

using namespace parabolic::translator;

namespace parabolic::media
{
    AudioLanguage::AudioLanguage(std::string language, bool audio_description)
        : m_language{ std::move(language) },
          m_audio_description{ audio_description }
    {
    }

    const std::string& AudioLanguage::language() const
    {
        return m_language;
    }

    bool AudioLanguage::has_audio_description() const
    {
        return m_audio_description;
    }

    Format::Format(std::string id, std::string protocol, Type type)
        : m_id{ std::move(id) },
          m_protocol{ std::move(protocol) },
          m_type{ type }
    {
    }

    Format& Format::best_video()
    {
        static Format format{ "BEST_VIDEO", "BEST", Type::Video };
        return format;
    }

    Format& Format::best_audio()
    {
        static Format format{ "BEST_AUDIO", "BEST", Type::Audio };
        return format;
    }

    Format& Format::worst_video()
    {
        static Format format{ "WORST_VIDEO", "WORST", Type::Video };
        return format;
    }

    Format& Format::worst_audio()
    {
        static Format format{ "WORST_AUDIO", "WORST", Type::Audio };
        return format;
    }

    Format& Format::none_video()
    {
        static Format format{ "NONE_VIDEO", "NONE", Type::Video };
        return format;
    }

    Format& Format::none_audio()
    {
        static Format format{ "NONE_AUDIO", "NONE", Type::Audio };
        return format;
    }

    const std::string& Format::id() const
    {
        return m_id;
    }

    const std::string& Format::protocol() const
    {
        return m_protocol;
    }

    const std::string& Format::extension() const
    {
        return m_extension;
    }

    unsigned long Format::bytes() const
    {
        return m_bytes;
    }

    Type Format::type() const
    {
        return m_type;
    }

    std::optional<double> Format::bitrate() const
    {
        return m_bitrate;
    }

    std::optional<AudioLanguage> Format::audio_language() const
    {
        return m_audio_language;
    }

    std::optional<VideoCodec> Format::video_codec() const
    {
        return m_video_codec;
    }

    std::optional<AudioCodec> Format::audio_codec() const
    {
        return m_audio_codec;
    }

    std::optional<VideoResolution> Format::video_resolution() const
    {
        return m_video_resolution;
    }

    std::optional<FrameRate> Format::frame_rate() const
    {
        return m_frame_rate;
    }

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

    std::optional<VideoResolution> VideoResolution::parse(std::string_view value)
    {
        if (value == "Best")
        {
            return best();
        }
        if (value == "Worst")
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
