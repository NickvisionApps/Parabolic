#include "ytdlp.h"
#include <cstddef>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <functional>
#include <reproc++/drain.hpp>
#include <reproc++/reproc.hpp>
#include <stop_token>
#include <string>
#include <system_error>
#include <tuple>
#include <utility>
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

    std::expected<std::string, std::string> execute(const std::vector<std::string>& args,
                                                    const std::function<void(const std::string&)>& output_callback,
                                                    const std::stop_token& stop_token)
    {
        if (stop_token.stop_requested())
        {
            return std::unexpected{ "Operation cancelled." };
        }
        std::vector<std::string> command{ executable_path() };
        command.append_range(args);
        reproc::options options;
        options.redirect.in.type = reproc::redirect::discard;
        reproc::process process;
        std::error_code ec{ process.start(command, options) };
        if (ec)
        {
            return std::unexpected{ ec.message() };
        }
        std::string output;
        std::string error;
        std::string pending;
        {
            std::stop_callback terminate_on_stop{ stop_token, [&process]
            {
                std::ignore = process.terminate();
            } };
            ec = reproc::drain(process,
                               [&output, &pending, &output_callback](reproc::stream, const uint8_t* buffer, size_t size)
            {
                output.append(reinterpret_cast<const char*>(buffer), size);
                if (output_callback)
                {
                    pending.append(reinterpret_cast<const char*>(buffer), size);
                    size_t newline{ pending.find('\n') };
                    while (newline != std::string::npos)
                    {
                        std::string line{ pending.substr(0, newline) };
                        pending.erase(0, newline + 1);
                        if (!line.empty() && line.back() == '\r')
                        {
                            line.pop_back();
                        }
                        output_callback(line);
                        newline = pending.find('\n');
                    }
                }
                return std::error_code{};
            }, reproc::sink::string(error));
        }
        if (ec)
        {
            std::ignore = process.kill();
            return std::unexpected{ ec.message() };
        }
        if (output_callback && !pending.empty())
        {
            output_callback(pending);
        }
        std::pair<int, std::error_code> status{ process.wait(reproc::infinite) };
        if (status.second)
        {
            std::ignore = process.kill();
            return std::unexpected{ status.second.message() };
        }
        if (stop_token.stop_requested())
        {
            return std::unexpected{ "Operation cancelled." };
        }
        if (status.first != 0)
        {
            return std::unexpected{ error };
        }
        return output;
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
