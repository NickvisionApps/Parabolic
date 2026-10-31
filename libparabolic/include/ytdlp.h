#ifndef YTDLP_H
#define YTDLP_H

#include <expected>
#include <string>
#include <vector>

namespace parabolic::ytdlp
{
    [[nodiscard]] std::string executable_path();
    [[nodiscard]] std::expected<std::string, std::string> execute(const std::vector<std::string>& args);
    [[nodiscard]] std::string plugins_path();
}

#endif
