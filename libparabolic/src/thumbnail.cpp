#include "thumbnail.h"
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace parabolic::thumbnail
{
    Thumbnail::Thumbnail(std::string url)
        : m_url{ std::move(url) }
    {
    }

    const std::string& Thumbnail::url() const
    {
        return m_url;
    }

    const std::vector<std::byte>& Thumbnail::bytes() const
    {
        static std::vector<std::byte> empty;
        return empty;
    }
}
