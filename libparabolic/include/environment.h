#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <cstdint>
#include <expected>
#include <filesystem>
#include <reproc++/drain.hpp>
#include <reproc++/reproc.hpp>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif

namespace parabolic::environment
{
    enum class DependencySearchOption : std::uint8_t
    {
        Global,
        App,
        System,
        Local,
    };

    enum class DeploymentMode : std::uint8_t
    {
        Local,
        Flatpak,
        Snap,
        Wsl,
        AppImage,
    };

    class Process
    {
    public:
        [[nodiscard]] static std::expected<Process, std::error_code> start(const std::vector<std::string>& command,
                                                                           const reproc::options& options);
        Process(const Process&) = delete;
        Process(Process&& other) noexcept;
        ~Process();
        void kill();
        [[nodiscard]] std::pair<int, std::error_code> wait();
        Process& operator=(const Process&) = delete;
        Process& operator=(Process&&) = delete;

        template<typename Out, typename Err>
        [[nodiscard]] std::error_code drain(Out&& out, Err&& err)
        {
            return reproc::drain(m_process, std::forward<Out>(out), std::forward<Err>(err));
        }

    private:
        Process() = default;
        reproc::process m_process;
#ifdef _WIN32
        HANDLE m_job{ nullptr };
#else
        int m_pid{ 0 };
#endif
    };

    const std::vector<std::string>& arguments();
    [[nodiscard]] DeploymentMode deployment_mode();
    [[nodiscard]] const std::filesystem::path& find_dependency(std::string_view name, DependencySearchOption option);
    [[nodiscard]] std::filesystem::path executable_directory();
    [[nodiscard]] std::filesystem::path executable_path();
    [[nodiscard]] bool is_portable();
    [[nodiscard]] std::string locale();
    [[nodiscard]] std::vector<std::filesystem::path> path();
    void set_variable(const std::string& name, const std::string& value);
    [[nodiscard]] std::string variable(const std::string& name);
}

#endif
