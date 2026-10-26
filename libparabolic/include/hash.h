#ifndef HASH_H
#define HASH_H

#include <filesystem>
#include <string>
#include <string_view>

namespace parabolic::hash
{
    std::string sha256_from_file(const std::filesystem::path& file_path);
    std::string sha256_from_string(std::string_view data);
    std::string sha512_from_file(const std::filesystem::path& file_path);
    std::string sha512_from_string(std::string_view data);
}

#endif
