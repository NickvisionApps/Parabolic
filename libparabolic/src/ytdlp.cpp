#include "ytdlp.h"
#include <cstddef>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <format>
#include <optional>
#include <reproc++/drain.hpp>
#include <reproc++/reproc.hpp>
#include <sago/platform_folders.h>
#include <stop_token>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>
#include "deno.h"
#include "environment.h"
#include "ffmpeg.h"
#include "keyring.h"
#ifdef _WIN32
#include "string_manip.h"
#endif

namespace parabolic::ytdlp
{
    Options::Options(const std::string& url, ProcessOutputCallback output_callback, std::stop_token stop_token)
        : m_arguments{ 12 },
          m_output_callback{ std::move(output_callback) },
          m_stop_token{ std::move(stop_token) }
    {
        std::filesystem::path plugins{ environment::executable_directory() / "plugins" };
        m_arguments.push_back(executable_path());
        m_arguments.push_back(url);
        m_arguments.emplace_back("--ignore-config");
        m_arguments.emplace_back("--no-warnings");
        m_arguments.emplace_back("--ffmpeg-location");
        m_arguments.push_back(ffmpeg::executable_path());
        m_arguments.emplace_back("--js-runtimes");
        m_arguments.push_back(std::format("deno:{}", deno::executable_path()));
        m_arguments.emplace_back("--paths");
        m_arguments.push_back(std::format("temp:{}", sago::getCacheDir()));
        if (std::filesystem::exists(plugins))
        {
            m_arguments.emplace_back("--plugins");
#ifdef _WIN32
            m_arguments.push_back(string_manip::str(plugins.wstring()));
#else
            m_arguments.push_back(plugins.string());
#endif
        }
    }

    const std::vector<std::string>& Options::arguments() const
    {
        return m_arguments;
    }

    const ProcessOutputCallback& Options::output_callback() const
    {
        return m_output_callback;
    }

    const std::stop_token& Options::stop_token() const
    {
        return m_stop_token;
    }

    void Options::add_argument(const std::string& argument)
    {
        m_arguments.push_back(argument);
    }

    void Options::add_arguments(const std::vector<std::string>& arguments)
    {
        m_arguments.append_range(arguments);
    }

    void Options::add_credential(const std::optional<keyring::Credential>& credential)
    {
        if (credential)
        {
            if (!credential->username().empty() && !credential->password().empty())
            {
                m_arguments.emplace_back("--username");
                m_arguments.push_back(credential->username());
                m_arguments.emplace_back("--password");
                m_arguments.push_back(credential->password());
            }
            else if (!credential->password().empty())
            {
                m_arguments.emplace_back("--video-password");
                m_arguments.push_back(credential->password());
            }
        }
    }

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

    std::expected<std::string, std::string> execute(const Options& options)
    {
        if (options.stop_token().stop_requested())
        {
            return std::unexpected{ "Operation cancelled." };
        }
        reproc::options proc_options;
        proc_options.redirect.in.type = reproc::redirect::discard;
        std::expected<environment::Process, std::error_code> started{
            environment::Process::start(options.arguments(), proc_options),
        };
        if (!started)
        {
            return std::unexpected{ started.error().message() };
        }
        environment::Process& process{ *started };
        std::string output;
        std::string error;
        std::string pending;
        std::error_code ec;
        {
            std::stop_callback kill_on_stop{ options.stop_token(), [&process]
            {
                process.kill();
            } };
            ec = process.drain([&options, &output, &pending](reproc::stream, const uint8_t* buffer, size_t size)
            {
                std::string_view chunk{ reinterpret_cast<const char*>(buffer), size };
                output.append(chunk);
                if (options.output_callback())
                {
                    pending.append(chunk);
                    size_t newline{ pending.find('\n') };
                    while (newline != std::string::npos)
                    {
                        std::string line{ pending.substr(0, newline) };
                        pending.erase(0, newline + 1);
                        if (!line.empty() && line.back() == '\r')
                        {
                            line.pop_back();
                        }
                        options.output_callback()(line);
                        newline = pending.find('\n');
                    }
                }
                return std::error_code{};
            }, reproc::sink::string(error));
        }
        if (ec)
        {
            return std::unexpected{ ec.message() };
        }
        if (options.output_callback() && !pending.empty())
        {
            options.output_callback()(pending);
        }
        std::pair<int, std::error_code> status{ process.wait() };
        if (status.second)
        {
            return std::unexpected{ status.second.message() };
        }
        if (options.stop_token().stop_requested())
        {
            return std::unexpected{ "Operation cancelled." };
        }
        if (status.first != 0)
        {
            return std::unexpected{ error };
        }
        return output;
    }
}
