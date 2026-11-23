#include "deno.h"
#include <filesystem>
#include <string>
#include "environment.h"
#ifdef _WIN32
#include "string_manip.h"
#endif

namespace parabolic::deno
{
    std::string executable_path()
    {
        const std::filesystem::path& global_path{
            environment::find_dependency("deno", environment::DependencySearchOption::Local),
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
            environment::find_dependency("deno", environment::DependencySearchOption::Global),
        };
        if (std::filesystem::exists(local_path))
        {
#ifdef _WIN32
            return string_manip::str(local_path.wstring());
#else
            return local_path.string();
#endif
        }
        return "deno";
    }
}
