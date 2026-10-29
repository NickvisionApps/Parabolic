#include "format.h"
#include <compare>
#include <format>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <utility>
#include "audio.h"
#include "file.h"
#include "string_manip.h"
#include "subtitle.h"
#include "translation.h"
#include "video.h"

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

    std::string strip_error_prefix(const std::string& message)
    {
        static std::string prefix{ "ERROR: " };
        if (message.starts_with(prefix))
        {
            return message.substr(prefix.size());
        }
        return message;
    }
}

namespace parabolic::format
{
    Format::Format(std::string id, std::string protocol, file::Kind kind)
        : m_id{ std::move(id) },
          m_protocol{ std::move(protocol) },
          m_kind{ kind }
    {
    }

    Format& Format::best_video()
    {
        static Format format{ "BEST_VIDEO", "BEST", file::Kind::Video };
        return format;
    }

    Format& Format::best_audio()
    {
        static Format format{ "BEST_AUDIO", "BEST", file::Kind::Audio };
        return format;
    }

    Format& Format::worst_video()
    {
        static Format format{ "WORST_VIDEO", "WORST", file::Kind::Video };
        return format;
    }

    Format& Format::worst_audio()
    {
        static Format format{ "WORST_AUDIO", "WORST", file::Kind::Audio };
        return format;
    }

    Format& Format::none_video()
    {
        static Format format{ "NONE_VIDEO", "NONE", file::Kind::Video };
        return format;
    }

    Format& Format::none_audio()
    {
        static Format format{ "NONE_AUDIO", "NONE", file::Kind::Audio };
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

    file::Kind Format::kind() const
    {
        return m_kind;
    }

    std::optional<double> Format::bitrate() const
    {
        return m_bitrate;
    }

    std::optional<audio::Language> Format::audio_language() const
    {
        return m_audio_language;
    }

    std::optional<video::Codec> Format::video_codec() const
    {
        return m_video_codec;
    }

    std::optional<audio::Codec> Format::audio_codec() const
    {
        return m_audio_codec;
    }

    std::optional<video::Resolution> Format::video_resolution() const
    {
        return m_video_resolution;
    }

    std::optional<video::FrameRate> Format::frame_rate() const
    {
        return m_frame_rate;
    }

    std::string Format::str(const translation::Translator& translator) const
    {
        std::string result;
        if (m_kind == file::Kind::Video)
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
                case video::Codec::Vp9:
                    append_format_section(result, "VP9");
                    break;
                case video::Codec::Av01:
                    append_format_section(result, "AV1");
                    break;
                case video::Codec::H264:
                    append_format_section(result, "H.264");
                    break;
                case video::Codec::H265:
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
                case video::FrameRate::Fps24:
                    append_format_section(result, translator._("{} FPS", 24));
                    break;
                case video::FrameRate::Fps30:
                    append_format_section(result, translator._("{} FPS", 30));
                    break;
                case video::FrameRate::Fps60:
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
                case audio::Codec::Flac:
                    append_format_section(result, "FLAC");
                    break;
                case audio::Codec::Wav:
                    append_format_section(result, "WAV");
                    break;
                case audio::Codec::Opus:
                    append_format_section(result, "OPUS");
                    break;
                case audio::Codec::Aac:
                    append_format_section(result, "AAC");
                    break;
                case audio::Codec::Mp4a:
                    append_format_section(result, "MP4A");
                    break;
                case audio::Codec::Mp3:
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
        else if (m_kind == file::Kind::Audio)
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
                case audio::Codec::Flac:
                    append_format_section(result, "FLAC");
                    break;
                case audio::Codec::Wav:
                    append_format_section(result, "WAV");
                    break;
                case audio::Codec::Opus:
                    append_format_section(result, "OPUS");
                    break;
                case audio::Codec::Aac:
                    append_format_section(result, "AAC");
                    break;
                case audio::Codec::Mp4a:
                    append_format_section(result, "MP4A");
                    break;
                case audio::Codec::Mp3:
                    append_format_section(result, "MP3");
                    break;
                default:
                    break;
                }
            }
        }
        else if (m_kind == file::Kind::Image && m_video_resolution)
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
        if (m_kind == file::Kind::Video)
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

    bool Options::include_super_resolutions() const
    {
        return m_include_super_resolutions;
    }

    void Options::set_include_super_resolutions(bool value)
    {
        m_include_super_resolutions = value;
    }

    audio::Codec Options::preferred_audio_codec() const
    {
        return m_preferred_audio_codec;
    }

    void Options::set_preferred_audio_codec(audio::Codec value)
    {
        m_preferred_audio_codec = value;
    }

    video::FrameRate Options::preferred_frame_rate() const
    {
        return m_preferred_frame_rate;
    }

    void Options::set_preferred_frame_rate(video::FrameRate value)
    {
        m_preferred_frame_rate = value;
    }

    subtitle::Type Options::preferred_subtitle_format() const
    {
        return m_preferred_subtitle_format;
    }

    void Options::set_preferred_subtitle_format(subtitle::Type value)
    {
        m_preferred_subtitle_format = value;
    }

    video::Codec Options::preferred_video_codec() const
    {
        return m_preferred_video_codec;
    }

    void Options::set_preferred_video_codec(video::Codec value)
    {
        m_preferred_video_codec = value;
    }

    void from_json(const nlohmann::json& json, Format& format)
    {
        if (json.contains("format_id") && json["format_id"].is_string())
        {
            format.m_id = json.at("format_id").get<std::string>();
        }
        if (json.contains("protocol") && json["protocol"].is_string())
        {
            format.m_protocol = json.at("protocol").get<std::string>();
        }
        if (json.contains("ext") && json["ext"].is_string())
        {
            format.m_extension = json.at("ext").get<std::string>();
        }
        if (json.contains("filesize") && json["filesize"].is_number_unsigned())
        {
            format.m_bytes = json.at("filesize").get<unsigned long>();
        }
        if (json.contains("tbr") && json["tbr"].is_number())
        {
            format.m_bitrate = json.at("tbr").get<double>();
        }
        std::string note;
        std::string resolution;
        if (json.contains("format_note") && json["format_note"].is_string())
        {
            note = json.at("format_note").get<std::string>();
        }
        if (json.contains("resolution") && json["resolution"].is_string())
        {
            resolution = json.at("resolution").get<std::string>();
        }
        if (resolution == "audio only")
        {
            format.m_kind = file::Kind::Audio;
        }
        else
        {
            format.m_kind = note == "storyboard" ? file::Kind::Image : file::Kind::Video;
            format.m_video_resolution = video::Resolution::parse(resolution);
        }
        if (json.contains("language") && json["language"].is_string() && !json["language"].get<std::string>().empty())
        {
            format.m_audio_language = { json.at("language").get<std::string>(),
                                        string_manip::lower(format.m_id).contains("audiodesc") };
        }
        if (json.contains("vcodec") && json["vcodec"].is_string())
        {
            std::string vcodec{ string_manip::lower(json.at("vcodec").get<std::string>()) };
            if (vcodec.contains("vp09") || vcodec.contains("vp9"))
            {
                format.m_video_codec = video::Codec::Vp9;
            }
            else if (vcodec.contains("av01"))
            {
                format.m_video_codec = video::Codec::Av01;
            }
            else if (vcodec.contains("avc1") || vcodec.contains("h264"))
            {
                format.m_video_codec = video::Codec::H264;
            }
            else if (vcodec.contains("hevc") || vcodec.contains("h265"))
            {
                format.m_video_codec = video::Codec::H265;
            }
        }
        if (json.contains("acodec") && json["acodec"].is_string())
        {
            std::string acodec{ string_manip::lower(json.at("acodec").get<std::string>()) };
            if (acodec.contains("flac") || acodec.contains("alac"))
            {
                format.m_audio_codec = audio::Codec::Flac;
            }
            else if (acodec.contains("wav") || acodec.contains("aiff"))
            {
                format.m_audio_codec = audio::Codec::Wav;
            }
            else if (acodec.contains("opus"))
            {
                format.m_audio_codec = audio::Codec::Opus;
            }
            else if (acodec.contains("aac"))
            {
                format.m_audio_codec = audio::Codec::Aac;
            }
            else if (acodec.contains("mp4a"))
            {
                format.m_audio_codec = audio::Codec::Mp4a;
            }
            else if (acodec.contains("mp3"))
            {
                format.m_audio_codec = audio::Codec::Mp3;
            }
        }
        if (json.contains("fps") && json["fps"].is_number())
        {
            double fps{ json.at("fps").get<double>() };
            if (fps == 24.0)
            {
                format.m_frame_rate = video::FrameRate::Fps24;
            }
            else if (fps == 30.0)
            {
                format.m_frame_rate = video::FrameRate::Fps30;
            }
            else if (fps == 60.0)
            {
                format.m_frame_rate = video::FrameRate::Fps60;
            }
        }
    }

    void from_json(const nlohmann::json& json, Options& options)
    {
        if (json.contains("includeSuperResolutions") && json["includeSuperResolutions"].is_boolean())
        {
            options.m_include_super_resolutions = json.at("includeSuperResolutions").get<bool>();
        }
        else if (json.contains("IncludeSuperResolutions") && json["IncludeSuperResolutions"].is_boolean())
        {
            options.m_include_super_resolutions = json.at("IncludeSuperResolutions").get<bool>();
        }
        if (json.contains("preferredAudioCodec") && json["preferredAudioCodec"].is_number_integer())
        {
            int value{ json.at("preferredAudioCodec").get<int>() };
            if (value >= static_cast<int>(audio::Codec::Any) && value <= static_cast<int>(audio::Codec::Mp3))
            {
                options.m_preferred_audio_codec = static_cast<audio::Codec>(value);
            }
        }
        else if (json.contains("PreferredAudioCodec") && json["PreferredAudioCodec"].is_number_integer())
        {
            int value{ json.at("PreferredAudioCodec").get<int>() };
            if (value >= static_cast<int>(audio::Codec::Any) && value <= static_cast<int>(audio::Codec::Mp3))
            {
                options.m_preferred_audio_codec = static_cast<audio::Codec>(value);
            }
        }
        if (json.contains("preferredFrameRate") && json["preferredFrameRate"].is_number_integer())
        {
            int value{ json.at("preferredFrameRate").get<int>() };
            if (value >= static_cast<int>(video::FrameRate::Any) && value <= static_cast<int>(video::FrameRate::Fps60))
            {
                options.m_preferred_frame_rate = static_cast<video::FrameRate>(value);
            }
        }
        else if (json.contains("PreferredFrameRate") && json["PreferredFrameRate"].is_number_integer())
        {
            int value{ json.at("PreferredFrameRate").get<int>() };
            if (value >= static_cast<int>(video::FrameRate::Any) && value <= static_cast<int>(video::FrameRate::Fps60))
            {
                options.m_preferred_frame_rate = static_cast<video::FrameRate>(value);
            }
        }
        if (json.contains("preferredSubtitleFormat") && json["preferredSubtitleFormat"].is_number_integer())
        {
            int value{ json.at("preferredSubtitleFormat").get<int>() };
            if (value >= static_cast<int>(subtitle::Type::Any) && value <= static_cast<int>(subtitle::Type::Lrc))
            {
                options.m_preferred_subtitle_format = static_cast<subtitle::Type>(value);
            }
        }
        else if (json.contains("PreferredSubtitleFormat") && json["PreferredSubtitleFormat"].is_number_integer())
        {
            int value{ json.at("PreferredSubtitleFormat").get<int>() };
            if (value >= static_cast<int>(subtitle::Type::Any) && value <= static_cast<int>(subtitle::Type::Lrc))
            {
                options.m_preferred_subtitle_format = static_cast<subtitle::Type>(value);
            }
        }
        if (json.contains("preferredVideoCodec") && json["preferredVideoCodec"].is_number_integer())
        {
            int value{ json.at("preferredVideoCodec").get<int>() };
            if (value >= static_cast<int>(video::Codec::Any) && value <= static_cast<int>(video::Codec::H264))
            {
                options.m_preferred_video_codec = static_cast<video::Codec>(value);
            }
        }
        else if (json.contains("PreferredVideoCodec") && json["PreferredVideoCodec"].is_number_integer())
        {
            int value{ json.at("PreferredVideoCodec").get<int>() };
            if (value >= static_cast<int>(video::Codec::Any) && value <= static_cast<int>(video::Codec::H264))
            {
                options.m_preferred_video_codec = static_cast<video::Codec>(value);
            }
        }
    }

    void to_json(nlohmann::json& json, const Format& format)
    {
        json = nlohmann::json{
            { "format_id", format.m_id },
            { "protocol", format.m_protocol },
            { "ext", format.m_extension },
            { "filesize", format.m_bytes },
            { "format_note", format.m_kind == file::Kind::Image ? "storyboard" : "" },
            { "resolution", "" },
            { "vcodec", "none" },
            { "acodec", "none" },
        };
        if (format.m_kind == file::Kind::Audio)
        {
            json["resolution"] = "audio only";
        }
        else if (format.m_video_resolution)
        {
            json["resolution"] =
                    std::format("{}x{}", format.m_video_resolution->width(), format.m_video_resolution->height());
        }
        if (format.m_bitrate)
        {
            json["tbr"] = *format.m_bitrate;
        }
        if (format.m_audio_language)
        {
            json["language"] = format.m_audio_language->language();
        }
        if (format.m_video_codec)
        {
            switch (*format.m_video_codec)
            {
            case video::Codec::Vp9:
                json["vcodec"] = "vp09";
                break;
            case video::Codec::Av01:
                json["vcodec"] = "av01";
                break;
            case video::Codec::H264:
                json["vcodec"] = "avc1";
                break;
            case video::Codec::H265:
                json["vcodec"] = "hevc";
                break;
            default:
                break;
            }
        }
        if (format.m_audio_codec)
        {
            switch (*format.m_audio_codec)
            {
            case audio::Codec::Flac:
                json["acodec"] = "flac";
                break;
            case audio::Codec::Wav:
                json["acodec"] = "wav";
                break;
            case audio::Codec::Opus:
                json["acodec"] = "opus";
                break;
            case audio::Codec::Aac:
                json["acodec"] = "aac";
                break;
            case audio::Codec::Mp4a:
                json["acodec"] = "mp4a";
                break;
            case audio::Codec::Mp3:
                json["acodec"] = "mp3";
                break;
            default:
                break;
            }
        }
        if (format.m_frame_rate)
        {
            switch (*format.m_frame_rate)
            {
            case video::FrameRate::Fps24:
                json["fps"] = 24.0;
                break;
            case video::FrameRate::Fps30:
                json["fps"] = 30.0;
                break;
            case video::FrameRate::Fps60:
                json["fps"] = 60.0;
                break;
            default:
                break;
            }
        }
    }

    void to_json(nlohmann::json& json, const Options& options)
    {
        json = nlohmann::json{
            { "includeSuperResolutions", options.m_include_super_resolutions },
            { "preferredAudioCodec", static_cast<int>(options.m_preferred_audio_codec) },
            { "preferredFrameRate", static_cast<int>(options.m_preferred_frame_rate) },
            { "preferredSubtitleFormat", static_cast<int>(options.m_preferred_subtitle_format) },
            { "preferredVideoCodec", static_cast<int>(options.m_preferred_video_codec) },
        };
    }
}
