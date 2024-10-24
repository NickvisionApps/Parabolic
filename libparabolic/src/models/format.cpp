#include "models/format.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    Format::Format(boost::json::object json, bool isYtdlpJson)
        : m_hasAudioDescription{ false }
    {
        if(isYtdlpJson)
        {
            m_id = json["format_id"].is_string() ? json["format_id"].as_string() : "";
            m_protocol = json["protocol"].is_string() ? json["protocol"].as_string() : "";
            m_extension = json["ext"].is_string() ? json["ext"].as_string() : "";
            std::string note{ json["format_note"].is_string() ? json["format_note"].as_string() : "" };
            std::string resolution{ json["resolution"].is_string() ? json["resolution"].as_string() : "" };
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
                double abr{ json["abr"].is_double() ? json["abr"].as_double() : 0.0 };
                if(abr > 0.0)
                {
                    m_audioBitrate = abr;
                }
            }
            else if(note == "storyboard")
            {
                m_type = MediaType::Image;
            }
            else
            {
                m_type = MediaType::Video;
                std::string vcodec{ json["vcodec"].is_string() ? json["vcodec"].as_string() : "" };
                if(!vcodec.empty())
                {
                    if(vcodec.find("vp09") != std::string::npos)
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
            m_type = json["Type"].is_int64() ? static_cast<MediaType>(json["Type"].as_int64()) : MediaType::Video;
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
            if(json["AudioBitrate"].is_double())
            {
                m_audioBitrate = json["AudioBitrate"].as_double();
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

    const std::optional<double>& Format::getAudioBitrate() const
    {
        return m_audioBitrate;
    }

    boost::json::object Format::toJson() const
    {
        boost::json::object json;
        json["Id"] = m_id;
        json["Protocol"] = m_protocol;
        json["Extension"] = m_extension;
        json["Type"] = static_cast<int>(m_type);
        if(m_audioLanguage)
        {
            json["AudioLanguage"] = m_audioLanguage.value();
        }
        json["HasAudioDescription"] = m_hasAudioDescription;
        if(m_videoCodec)
        {
            json["VideoCodec"] = static_cast<int>(m_videoCodec.value());
        }
        if(m_videoResolution)
        {
            json["VideoResolution"] = m_videoResolution.value().toJson();
        }
        if(m_audioBitrate)
        {
            json["AudioBitrate"] = m_audioBitrate.value();
        }
        return json;
    }
}