#include "models/format.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    Format::Format(boost::json::object format)
        : m_hasAudioDescription{ false }
    {
        m_id = format["format_id"].is_string() ? format["format_id"].as_string() : "";
        m_protocol = format["protocol"].is_string() ? format["protocol"].as_string() : "";
        m_extension = format["ext"].is_string() ? format["ext"].as_string() : "";
        std::string note{ format["format_note"].is_string() ? format["format_note"].as_string() : "" };
        std::string resolution{ format["resolution"].is_string() ? format["resolution"].as_string() : "" };
        if(resolution == "audio only")
        {
            m_type = MediaType::Audio;
            std::string language{ format["language"].is_string() ? format["language"].as_string() : "" };
            if(!language.empty())
            {
                m_audioLanguage = language;
                if(m_id.find("audiodesc") != std::string::npos)
                {
                    m_hasAudioDescription = true;
                }
            }
            double abr{ format["abr"].is_double() ? format["abr"].as_double() : 0.0 };
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
            std::string vcodec{ format["vcodec"].is_string() ? format["vcodec"].as_string() : "" };
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
}