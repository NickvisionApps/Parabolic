#ifndef COOKIES_H
#define COOKIES_H

#include <cstdint>
#include <string>

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

    [[nodiscard]] std::string to_argument_str(Browser browser);
}

#endif
