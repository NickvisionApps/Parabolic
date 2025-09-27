#ifndef BROWSERS_H
#define BROWSERS_H

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief Types of browsers that can be used to load cookies.
     */
    enum class Browser
    {
        None = 0,
        Brave,
        Chrome,
        Chromium,
        Edge,
        Firefox,
        Opera,
        Vivaldi,
        Whale
    };
}

#endif //BROWSERS_H
