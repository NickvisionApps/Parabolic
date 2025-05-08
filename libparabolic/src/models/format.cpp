#include "models/format.h"
#include <cmath>
#include <sstream>
#include <libnick/localization/gettext.h>

namespace Nickvision::TubeConverter::Shared::Models
{
    Format::Format(boost::json::object json, bool isYtdlpJson)
        : m_bytes{ 0 },
        m_hasAudioDescription{ false }
    {
        if(isYtdlpJson)
        {
            m_id = json["format_id"].is_string() ? json["format_id"].as_string() : "";
            m_protocol = json["protocol"].is_string() ? json["protocol"].as_string() : "";
            m_extension = json["ext"].is_string() ? json["ext"].as_string() : "";
            m_bytes = json["filesize"].is_int64() ? static_cast<unsigned long long>(json["filesize"].as_int64()) : 0;
            double bitrate{ json["tbr"].is_double() ? json["tbr"].as_double() : 0.0 };
            std::string note{ json["format_note"].is_string() ? json["format_note"].as_string() : "" };
            std::string resolution{ json["resolution"].is_string() ? json["resolution"].as_string() : "" };
            if(bitrate > 0)
            {
                m_bitrate = bitrate;
            }
            if(resolution == "audio only")
            {
                m_type = MediaType::Audio;
                std::string language{ json["language"].is_string() ? json["language"].as_string() : "" };
                if(!language.empty())
                {
                    m_audioLanguage = language;
                    if(m_id.find("audiodesc") != std::string::npos)
                    {
                        m_hasAudioDescription = true;
                    }
                }
            }
            else if(note == "storyboard")
            {
                m_type = MediaType::Image;
                m_videoResolution = VideoResolution::parse(resolution);
            }
            else
            {
                m_type = MediaType::Video;
                std::string vcodec{ json["vcodec"].is_string() ? json["vcodec"].as_string() : "" };
                if(!vcodec.empty())
                {
                    if(vcodec.find("vp09") != std::string::npos || vcodec.find("vp9") != std::string::npos)
                    {
                        m_videoCodec = VideoCodec::VP9;
                    }
                    else if(vcodec.find("av01") != std::string::npos)
                    {
                        m_videoCodec = VideoCodec::AV01;
                    }
                    else if(vcodec.find("avc1") != std::string::npos)
                    {
                        m_videoCodec = VideoCodec::H264;
                    }
                }
                m_videoResolution = VideoResolution::parse(resolution);
            }
        }
        else
        {
            m_id = json["Id"].is_string() ? json["Id"].as_string() : "";
            m_protocol = json["Protocol"].is_string() ? json["Protocol"].as_string() : "";
            m_extension = json["Extension"].is_string() ? json["Extension"].as_string() : "";
            m_bytes = json["Bytes"].is_uint64() ? json["Bytes"].as_uint64() : 0;
            m_type = json["Type"].is_int64() ? static_cast<MediaType>(json["Type"].as_int64()) : MediaType::Video;
            if(json["Bitrate"].is_double())
            {
                m_bitrate = json["Bitrate"].as_double();
            }
            if(json["AudioLanguage"].is_string())
            {
                m_audioLanguage = json["AudioLanguage"].as_string();
            }
            m_hasAudioDescription = json["HasAudioDescription"].is_bool() ? json["HasAudioDescription"].as_bool() : false;
            if(json["VideoCodec"].is_int64())
            {
                m_videoCodec = static_cast<VideoCodec>(json["VideoCodec"].as_int64());
            }
            if(json["VideoResolution"].is_object())
            {
                m_videoResolution = VideoResolution(json["VideoResolution"].as_object());
            }
        }
    }

    const std::string& Format::getId() const
    {
        return m_id;
    }

    const std::string& Format::getProtocol() const
    {
        return m_protocol;
    }

    const std::string& Format::getExtension() const
    {
        return m_extension;
    }

    MediaType Format::getType() const
    {
        return m_type;
    }

    const std::optional<double>& Format::getBitrate() const
    {
        return m_bitrate;
    }

    const std::optional<std::string>& Format::getAudioLanguage() const
    {
        return m_audioLanguage;
    }

    bool Format::hasAudioDescription() const
    {
        return m_hasAudioDescription;
    }

    const std::optional<VideoCodec>& Format::getVideoCodec() const
    {
        return m_videoCodec;
    }

    const std::optional<VideoResolution>& Format::getVideoResolution() const
    {
        return m_videoResolution;
    }

    std::string Format::str() const
    {
        std::stringstream builder;
        std::string separator{ " | " };
        if(m_type == MediaType::Video)
        {
            if(m_videoResolution)
            {
                builder << separator << m_videoResolution->str();
            }
            if(m_bitrate)
            {
                builder << separator << std::round(*m_bitrate) << "k";
            }
            if(m_videoCodec)
            {
                switch(*m_videoCodec)
                {
                case VideoCodec::VP9:
                    builder << separator << "VP9";
                    break;
                case VideoCodec::AV01:
                    builder << separator << "AV01";
                    break;
                case VideoCodec::H264:
                    builder << separator << "H.264";
                    break;
                }
            }
        }
        else if(m_type == MediaType::Audio)
        {
            if(m_bitrate)
            {
                builder << separator << std::round(*m_bitrate) << "k";
            }
            if(m_audioLanguage)
            {
                builder << separator << *m_audioLanguage;
                if(m_hasAudioDescription)
                {
                    builder << " (" << _("Audio Description") << ")";
                }
            }
        }
        else if(m_type == MediaType::Image)
        {
            if(m_videoResolution)
            {
                builder << separator << m_videoResolution->str();
            }
        }
        if(m_bytes > 0)
        {
            static constexpr double pow2{ 1024 * 1024 };
            static constexpr double pow3{ 1024 * 1024 * 1024 };
            builder << separator;
            if(m_bytes > pow3)
            {
                builder << _f("{:.2f} GiB", m_bytes / pow3);
            }
            else if(m_bytes > pow2)
            {
                builder << _f("{:.2f} MiB", m_bytes / pow2);
            }
            else if(m_bytes > 1024)
            {
                builder << _f("{:.2f} KiB", m_bytes / 1024.0);
            }
            else
            {
                builder << _f("{:.2f} B", m_bytes);
            }
        }
        builder << " (" << m_id << ")";
        std::string str{ builder.str() };
        if(str[1] == '|')
        {
            return str.substr(3);
        }
        else if(str[0] == ' ')
        {
            return str.substr(1);
        }
        return str;
    }

    boost::json::object Format::toJson() const
    {
        boost::json::object json;
        json["Id"] = m_id;
        json["Protocol"] = m_protocol;
        json["Extension"] = m_extension;
        json["Bytes"] = m_bytes;
        json["Type"] = static_cast<int>(m_type);
        if(m_bitrate)
        {
            json["Bitrate"] = *m_bitrate;
        }
        if(m_audioLanguage)
        {
            json["AudioLanguage"] = *m_audioLanguage;
        }
        json["HasAudioDescription"] = m_hasAudioDescription;
        if(m_videoCodec)
        {
            json["VideoCodec"] = static_cast<int>(*m_videoCodec);
        }
        if(m_videoResolution)
        {
            json["VideoResolution"] = m_videoResolution->toJson();
        }
        return json;
    }

    bool Format::operator==(const Format& format) const
    {
        return m_id == format.m_id;
    }

    bool Format::operator!=(const Format& format) const
    {
        return !(operator==(format));
    }

    bool Format::operator<(const Format& format) const
    {
        if(m_type == MediaType::Video && format.m_type == MediaType::Audio)
        {
            return true;
        }
        else if(m_type == MediaType::Audio && format.m_type == MediaType::Video)
        {
            return false;
        }
        else
        {
            if(m_type == MediaType::Video)
            {
                if(m_videoResolution && format.m_videoResolution)
                {
                    return *m_videoResolution < *format.m_videoResolution;
                }
                else if(m_videoResolution && !format.m_videoResolution)
                {
                    return true;
                }
                else if(!m_videoResolution && format.m_videoResolution)
                {
                    return false;
                }
                if(m_bitrate && format.m_bitrate)
                {
                    return *m_bitrate < *format.m_bitrate;
                }
                else if(m_bitrate && !format.m_bitrate)
                {
                    return true;
                }
                else if(!m_bitrate && format.m_bitrate)
                {
                    return false;
                }
                return m_id < format.m_id;
            }
            else if(m_type == MediaType::Audio)
            {
                if(m_bitrate && format.m_bitrate)
                {
                    return *m_bitrate < *format.m_bitrate;
                }
                else if(m_bitrate && !format.m_bitrate)
                {
                    return true;
                }
                else if(!m_bitrate && format.m_bitrate)
                {
                    return false;
                }
                return m_id < format.m_id;
            }
        }
        return m_id < format.m_id;
    }

    bool Format::operator>(const Format& format) const
    {
        return operator!=(format) && !(operator<(format));
    }
}
