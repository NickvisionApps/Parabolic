#ifndef URL_H
#define URL_H

#include <string>

namespace parabolic::url
{
    [[nodiscard]] bool is_valid(const std::string& url);
}

#endif
