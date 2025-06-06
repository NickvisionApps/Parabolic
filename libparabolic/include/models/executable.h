#ifndef EXECUTABLE_H
#define EXECUTABLE_H

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief Executables exposed by yt-dlp.
     */
    enum class Executable
    {
        None,
        AtomicParsley,
        FFmpeg,
        FFprobe
    };
}

#endif //EXECUTABLE_H
