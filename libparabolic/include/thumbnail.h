#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace parabolic::thumbnail
{
    class Thumbnail
    {
    public:
        Thumbnail(std::string url);
        [[nodiscard]] const std::string& url() const;
        [[nodiscard]] const std::vector<std::byte>& bytes() const;

    private:
        std::string m_url;
    };
}
