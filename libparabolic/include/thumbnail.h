#ifndef THUMBNAIL_H
#define THUMBNAIL_H

#include <cstdint>
#include <string>
#include <vector>

namespace parabolic::thumbnail
{
    class Image
    {
    public:
        Image(std::string url);
        [[nodiscard]] const std::string& url() const;
        [[nodiscard]] const std::vector<std::uint8_t>& bytes() const;

    private:
        std::string m_url;
    };
}

#endif
