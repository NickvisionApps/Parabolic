#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>

namespace parabolic::ytdlp
{
    enum class Browser : std::uint8_t
    {
        None,
        Brave,
        Chrome,
        Chromium,
        Edge,
        Firefox,
        Opera,
        Vivaldi,
        Whale,
    };

    class YtdlpException : public std::runtime_error
    {
    public:
        YtdlpException(const std::string& message);
    };

    bool is_youtube_supported_language(const std::string& language);
}
