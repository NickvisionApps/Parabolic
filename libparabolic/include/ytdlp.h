#ifndef YTDLP_H
#define YTDLP_H

#include <expected>
#include <functional>
#include <string>
#include <vector>

namespace parabolic::ytdlp
{
    [[nodiscard]] std::string executable_path();
    [[nodiscard]] std::expected<std::string, std::string>
    execute(const std::vector<std::string>& args, const std::function<void(const std::string&)>& output_callback = {});
    [[nodiscard]] std::string plugins_path();
}

#endif
