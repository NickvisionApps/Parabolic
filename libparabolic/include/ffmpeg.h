#ifndef FFMPEG_H
#define FFMPEG_H

#include <filesystem>
#include <string>

namespace parabolic::ffmpeg
{
    [[nodiscard]] std::string executable_path(const std::filesystem::path& resource_directory);
}

#endif
