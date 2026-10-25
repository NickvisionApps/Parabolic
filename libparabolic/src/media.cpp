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

namespace
{
    void append_format_section(std::string& result, const std::string& value)
    {
        if (value.empty())
        {
            return;
        }
        if (!result.empty())
        {
            result += " | ";
        }
        result += value;
    }
}

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

    std::string Format::str(const Translator& translator) const
    {
        std::string result;
        if (m_type == Type::Video)
        {
            if (m_video_resolution)
            {
                append_format_section(result, m_video_resolution->str(translator));
            }
            if (m_audio_language)
            {
                std::string language{ m_audio_language->language() };
                if (m_audio_language->has_audio_description())
                {
                    language += std::format(" ({})", translator._("Audio Description"));
                }
                append_format_section(result, language);
            }
            if (m_video_codec)
            {
                switch (*m_video_codec)
                {
                case VideoCodec::Vp9:
                    append_format_section(result, "VP9");
                    break;
                case VideoCodec::Av01:
                    append_format_section(result, "AV1");
                    break;
                case VideoCodec::H264:
                    append_format_section(result, "H.264");
                    break;
                case VideoCodec::H265:
                    append_format_section(result, "H.265");
                    break;
                default:
                    break;
                }
            }
            if (m_frame_rate)
            {
                switch (*m_frame_rate)
                {
                case FrameRate::Fps24:
                    append_format_section(result, translator._("{} FPS", 24));
                    break;
                case FrameRate::Fps30:
                    append_format_section(result, translator._("{} FPS", 30));
                    break;
                case FrameRate::Fps60:
                    append_format_section(result, translator._("{} FPS", 60));
                    break;
                default:
                    break;
                }
            }
            if (m_audio_codec)
            {
                switch (*m_audio_codec)
                {
                case AudioCodec::Flac:
                    append_format_section(result, "FLAC");
                    break;
                case AudioCodec::Wav:
                    append_format_section(result, "WAV");
                    break;
                case AudioCodec::Opus:
                    append_format_section(result, "OPUS");
                    break;
                case AudioCodec::Aac:
                    append_format_section(result, "AAC");
                    break;
                case AudioCodec::Mp4a:
                    append_format_section(result, "MP4A");
                    break;
                case AudioCodec::Mp3:
                    append_format_section(result, "MP3");
                    break;
                default:
                    break;
                }
            }
            if (m_bitrate)
            {
                append_format_section(result, std::format("{}k", *m_bitrate));
            }
        }
        else if (m_type == Type::Audio)
        {
            if (m_bitrate)
            {
                append_format_section(result, std::format("{}k", *m_bitrate));
            }
            if (m_audio_language)
            {
                std::string language{ m_audio_language->language() };
                if (m_audio_language->has_audio_description())
                {
                    language += std::format(" ({})", translator._("Audio Description"));
                }
                append_format_section(result, language);
            }
            if (m_audio_codec)
            {
                switch (*m_audio_codec)
                {
                case AudioCodec::Flac:
                    append_format_section(result, "FLAC");
                    break;
                case AudioCodec::Wav:
                    append_format_section(result, "WAV");
                    break;
                case AudioCodec::Opus:
                    append_format_section(result, "OPUS");
                    break;
                case AudioCodec::Aac:
                    append_format_section(result, "AAC");
                    break;
                case AudioCodec::Mp4a:
                    append_format_section(result, "MP4A");
                    break;
                case AudioCodec::Mp3:
                    append_format_section(result, "MP3");
                    break;
                default:
                    break;
                }
            }
        }
        else if (m_type == Type::Image && m_video_resolution)
        {
            append_format_section(result, m_video_resolution->str(translator));
        }
        if (m_bytes > 0)
        {
            constexpr double GIB{ 1024.0 * 1024.0 * 1024.0 };
            constexpr double MIB{ 1024.0 * 1024.0 };
            double bytes{ static_cast<double>(m_bytes) };
            if (bytes > GIB)
            {
                append_format_section(result, translator._("{:.2f} GiB", bytes / GIB));
            }
            else if (bytes > MIB)
            {
                append_format_section(result, translator._("{:.2f} MiB", bytes / MIB));
            }
            else if (bytes > 1024.0)
            {
                append_format_section(result, translator._("{:.2f} KiB", bytes / 1024.0));
            }
            else
            {
                append_format_section(result, translator._("{:.2f} B", bytes));
            }
        }
        std::string id_label{ m_id };
        if (m_id == "BEST_VIDEO" || m_id == "BEST_AUDIO")
        {
            id_label = translator._("Best");
        }
        else if (m_id == "WORST_VIDEO" || m_id == "WORST_AUDIO")
        {
            id_label = translator._("Worst");
        }
        else if (m_id == "NONE_VIDEO" || m_id == "NONE_AUDIO")
        {
            id_label = translator._("None");
        }
        if (result.empty())
        {
            return id_label;
        }
        return std::format("{} ({})", result, id_label);
    }

    std::strong_ordering Format::operator<=>(const Format& rhs) const
    {
        std::strong_ordering resolution_compare{ std::strong_ordering::equal };
        if (m_video_resolution && rhs.m_video_resolution)
        {
            resolution_compare = *m_video_resolution <=> *rhs.m_video_resolution;
        }
        else if (m_video_resolution)
        {
            resolution_compare = std::strong_ordering::greater;
        }
        std::strong_ordering language_compare{
            string_manip::lower(m_audio_language ? m_audio_language->language() : "") <=>
                    string_manip::lower(rhs.m_audio_language ? rhs.m_audio_language->language() : ""),
        };
        std::strong_ordering bitrate_compare{ std::strong_ordering::equal };
        if (m_bitrate && rhs.m_bitrate)
        {
            if (*m_bitrate < *rhs.m_bitrate)
            {
                bitrate_compare = std::strong_ordering::less;
            }
            else if (*m_bitrate > *rhs.m_bitrate)
            {
                bitrate_compare = std::strong_ordering::greater;
            }
        }
        if (resolution_compare != 0)
        {
            return resolution_compare;
        }
        if (m_type == Type::Video)
        {
            if (language_compare != 0)
            {
                return language_compare;
            }
            if (bitrate_compare != 0)
            {
                return bitrate_compare;
            }
            return m_id <=> rhs.m_id;
        }
        if (bitrate_compare != 0)
        {
            return bitrate_compare;
        }
        if (language_compare != 0)
        {
            return language_compare;
        }
        return m_id <=> rhs.m_id;
    }

    bool Format::operator==(const Format& rhs) const
    {
        return m_id == rhs.m_id;
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
