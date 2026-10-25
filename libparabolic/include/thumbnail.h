#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace parabolic::thumbnail
{
    class Thumbnail
    {
    public:
        Thumbnail(std::string url);
        [[nodiscard]] const std::string& url() const;
        [[nodiscard]] const std::vector<std::uint8_t>& bytes() const;

    private:
        std::string m_url;
    };
}
