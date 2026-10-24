#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace parabolic
{
    namespace environment
    {
        enum class DependencySearchOption : std::uint8_t
        {
            Global,
            App,
            System,
            Local
        };

        enum class DeploymentMode : std::uint8_t
        {
            Local,
            Flatpak,
            Snap,
            Wsl,
            AppImage,
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
}
