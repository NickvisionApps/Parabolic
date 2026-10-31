#include "ytdlp.h"
#include <expected>
#include <filesystem>
#include <string>
#include <vector>
#include "environment.h"
#ifdef _WIN32
#include "string_manip.h"
#endif

namespace parabolic::ytdlp
{
    std::string executable_path()
    {
        const std::filesystem::path& global_path{
            environment::find_dependency("yt-dlp", environment::DependencySearchOption::Local),
        };
        if (std::filesystem::exists(global_path))
        {
#ifdef _WIN32
            return string_manip::str(global_path.wstring());
#else
            return global_path.string();
#endif
        }
        const std::filesystem::path& local_path{
            environment::find_dependency("yt-dlp", environment::DependencySearchOption::Global),
        };
        if (std::filesystem::exists(local_path))
        {
#ifdef _WIN32
            return string_manip::str(local_path.wstring());
#else
            return local_path.string();
#endif
        }
        return "yt-dlp";
    }

    std::expected<std::string, std::string> execute(const std::vector<std::string>& args)
    {
        return {};
    }

    std::string plugins_path()
    {
        std::filesystem::path plugins{ environment::executable_directory() / "plugins" };
        if (std::filesystem::exists(plugins))
        {
#ifdef _WIN32
            return string_manip::str(plugins.wstring());
#else
            return plugins.string();
#endif
        }
        return {};
    }
}
