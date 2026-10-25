#pragma once

#include <cstdint>
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

    bool is_youtube_supported_language(const std::string& language);
}
