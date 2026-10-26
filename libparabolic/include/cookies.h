#ifndef COOKIES_H
#define COOKIES_H

#include <cstdint>

namespace parabolic::cookies
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

#endif
