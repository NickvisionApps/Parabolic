#pragma once

#include <cstdint>

namespace parabolic::browser
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
}
