#include "file.h"
#include <chrono>
#include <compare>
#include <format>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include "audio.h"
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
}

namespace parabolic::file
{
    Format::Format(std::string id, std::string protocol, Kind kind)
        : m_id{ std::move(id) },
          m_protocol{ std::move(protocol) },
          m_kind{ kind }
    {
    }

    Format& Format::best_video()
    {
        static Format format{ "BEST_VIDEO", "BEST", Kind::Video };
        return format;
    }

    Format& Format::best_audio()
    {
        static Format format{ "BEST_AUDIO", "BEST", Kind::Audio };
        return format;
    }

    Format& Format::worst_video()
    {
        static Format format{ "WORST_VIDEO", "WORST", Kind::Video };
        return format;
    }

    Format& Format::worst_audio()
    {
        static Format format{ "WORST_AUDIO", "WORST", Kind::Audio };
        return format;
    }

    Format& Format::none_video()
    {
        static Format format{ "NONE_VIDEO", "NONE", Kind::Video };
        return format;
    }

    Format& Format::none_audio()
    {
        static Format format{ "NONE_AUDIO", "NONE", Kind::Audio };
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

    Kind Format::kind() const
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
        if (m_kind == Kind::Video)
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
        else if (m_kind == Kind::Audio)
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
        else if (m_kind == Kind::Image && m_video_resolution)
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
        if (m_kind == Kind::Video)
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

    std::string_view dot_extension(Type type)
    {
        switch (type)
        {
        case Type::Mp4:
            return ".mp4";
        case Type::Webm:
            return ".webm";
        case Type::Mkv:
            return ".mkv";
        case Type::Mov:
            return ".mov";
        case Type::Avi:
            return ".avi";
        case Type::Mp3:
            return ".mp3";
        case Type::M4a:
            return ".m4a";
        case Type::Opus:
            return ".opus";
        case Type::Flac:
            return ".flac";
        case Type::Wav:
            return ".wav";
        case Type::Ogg:
            return ".ogg";
        default:
            return "";
        }
    }

    bool is_audio(Type type)
    {
        switch (type)
        {
        case Type::AudioGeneric:
        case Type::Mp3:
        case Type::M4a:
        case Type::Opus:
        case Type::Flac:
        case Type::Wav:
        case Type::Ogg:
            return true;
        default:
            return false;
        }
    }

    bool is_generic(Type type)
    {
        switch (type)
        {
        case Type::AudioGeneric:
        case Type::VideoGeneric:
            return true;
        default:
            return false;
        }
    }

    bool is_video(Type type)
    {
        switch (type)
        {
        case Type::VideoGeneric:
        case Type::Mp4:
        case Type::Webm:
        case Type::Mkv:
        case Type::Mov:
        case Type::Avi:
            return true;
        default:
            return false;
        }
    }

    bool should_recode(Type type)
    {
        switch (type)
        {
        case Type::Webm:
        case Type::Mov:
        case Type::Avi:
            return true;
        default:
            return false;
        }
    }

    bool supports_subtitle(Type type, subtitle::Type subtitle_type)
    {
        switch (subtitle_type)
        {
        case subtitle::Type::Any:
        case subtitle::Type::Vtt:
            return is_video(type) && type != Type::Avi;
        case subtitle::Type::Srt:
            return is_video(type) && type != Type::Webm && type != Type::Avi;
        case subtitle::Type::Ass:
            return is_video(type) && type != Type::Mkv;
        case subtitle::Type::Lrc:
            return is_audio(type);
        default:
            return false;
        }
    }

    bool supports_thumbnails(Type type)
    {
        switch (type)
        {
        case Type::Mp4:
        case Type::Mkv:
        case Type::Mov:
        case Type::Mp3:
        case Type::M4a:
        case Type::Opus:
        case Type::Flac:
        case Type::Ogg:
            return true;
        default:
            return false;
        }
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
            format.m_kind = Kind::Audio;
        }
        else
        {
            format.m_kind = note == "storyboard" ? Kind::Image : Kind::Video;
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

    void from_json(const nlohmann::json& json, TimeFrame& time_frame)
    {
        if (json.contains("start") && json["start"].is_number_integer())
        {
            time_frame.m_start = std::chrono::seconds(json.at("start").get<int>());
        }
        else if (json.contains("Start") && json["Start"].is_number_integer())
        {
            time_frame.m_start = std::chrono::seconds(json.at("Start").get<int>());
        }
        if (json.contains("end") && json["end"].is_number_integer())
        {
            time_frame.m_end = std::chrono::seconds(json.at("end").get<int>());
        }
        else if (json.contains("End") && json["End"].is_number_integer())
        {
            time_frame.m_end = std::chrono::seconds(json.at("End").get<int>());
        }
    }

    void to_json(nlohmann::json& json, const Format& format)
    {
        json = nlohmann::json{
            { "format_id", format.m_id },
            { "protocol", format.m_protocol },
            { "ext", format.m_extension },
            { "filesize", format.m_bytes },
            { "format_note", format.m_kind == Kind::Image ? "storyboard" : "" },
            { "resolution", "" },
            { "vcodec", "none" },
            { "acodec", "none" },
        };
        if (format.m_kind == Kind::Audio)
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

    void to_json(nlohmann::json& json, const TimeFrame& time_frame)
    {
        json = nlohmann::json{ { "start", time_frame.m_start.count() }, { "end", time_frame.m_end.count() } };
    }
}
