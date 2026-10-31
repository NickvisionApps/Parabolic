#include "ffmpeg.h"
#include <filesystem>
#include <string>
#include "environment.h"
#ifdef _WIN32
#include "string_manip.h"
#endif

namespace parabolic::ffmpeg
{
    std::string executable_path()
    {
        const std::filesystem::path& global_path{
            environment::find_dependency("ffmpeg", environment::DependencySearchOption::Global),
        };
        if (std::filesystem::exists(global_path))
        {
#ifdef _WIN32
            return string_manip::str(global_path.wstring());
#else
            return global_path.string();
#endif
        }
        return "ffmpeg";
    }
}
