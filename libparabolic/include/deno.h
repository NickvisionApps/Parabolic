#ifndef DENO_H
#define DENO_H

#include <filesystem>
#include <string>

namespace parabolic::deno
{
    [[nodiscard]] std::string executable_path(const std::filesystem::path& resource_directory);
}

#endif
