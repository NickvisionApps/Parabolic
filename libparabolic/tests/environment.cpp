#include <cstdint>
#include <filesystem>
#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "environment.h"

#ifdef _WIN32
#define REAL_BINARY "cmd.exe"
#define PATH_SEPARATOR ";"
#else
#define REAL_BINARY "ls"
#define PATH_SEPARATOR ":"
#endif

using namespace parabolic::environment;

namespace
{
    class EnvironmentVariableGuard
    {
    public:
        EnvironmentVariableGuard(std::string name)
            : m_name{ std::move(name) },
            m_value{ variable(m_name) }
        {
        }

        ~EnvironmentVariableGuard()
        {
            set_variable(m_name, m_value);
        }

        EnvironmentVariableGuard(const EnvironmentVariableGuard&) = delete;
        EnvironmentVariableGuard& operator=(const EnvironmentVariableGuard&) = delete;

    private:
        std::string m_name;
        std::string m_value;
    };
}

TEST(EnvironmentTest, arguments_not_empty)
{
    ASSERT_FALSE(arguments().empty());
}

TEST(EnvironmentTest, arguments_first_not_empty)
{
    ASSERT_FALSE(arguments().front().empty());
}

TEST(EnvironmentTest, arguments_cached)
{
    ASSERT_EQ(&arguments(), &arguments());
}

TEST(EnvironmentTest, is_portable_false)
{
    ASSERT_FALSE(is_portable());
}

TEST(EnvironmentTest, executable_path_exists)
{
    std::filesystem::path path{ executable_path() };
    ASSERT_FALSE(path.empty());
    ASSERT_TRUE(path.is_absolute());
    ASSERT_TRUE(std::filesystem::is_regular_file(path));
}

TEST(EnvironmentTest, executable_directory_is_parent)
{
    ASSERT_EQ(executable_path().parent_path(), executable_directory());
    ASSERT_TRUE(std::filesystem::is_directory(executable_directory()));
}

TEST(EnvironmentTest, variable_set_get)
{
    set_variable("PARABOLIC_TEST_VARIABLE", "value");
    ASSERT_EQ("value", variable("PARABOLIC_TEST_VARIABLE"));
}

TEST(EnvironmentTest, variable_overwrite)
{
    set_variable("PARABOLIC_TEST_VARIABLE", "first");
    set_variable("PARABOLIC_TEST_VARIABLE", "second");
    ASSERT_EQ("second", variable("PARABOLIC_TEST_VARIABLE"));
}

TEST(EnvironmentTest, variable_unset)
{
    ASSERT_EQ("", variable("PARABOLIC_TEST_UNSET_VARIABLE"));
}

TEST(EnvironmentTest, variable_empty_value)
{
    set_variable("PARABOLIC_TEST_VARIABLE", "");
    ASSERT_EQ("", variable("PARABOLIC_TEST_VARIABLE"));
}

TEST(EnvironmentTest, variable_utf8)
{
    set_variable("PARABOLIC_TEST_VARIABLE", "caf\xC3\xA9");
    ASSERT_EQ("caf\xC3\xA9", variable("PARABOLIC_TEST_VARIABLE"));
}

TEST(EnvironmentTest, path_split)
{
    EnvironmentVariableGuard guard{ "PATH" };
    set_variable("PATH", "a" PATH_SEPARATOR "b" PATH_SEPARATOR "c");
    std::vector<std::filesystem::path> expected{ "a", "b", "c" };
    ASSERT_EQ(expected, path());
}

TEST(EnvironmentTest, path_skips_empty_entries)
{
    EnvironmentVariableGuard guard{ "PATH" };
    set_variable("PATH", "a" PATH_SEPARATOR PATH_SEPARATOR " " PATH_SEPARATOR "b" PATH_SEPARATOR);
    std::vector<std::filesystem::path> expected{ "a", "b" };
    ASSERT_EQ(expected, path());
}

TEST(EnvironmentTest, path_empty)
{
    EnvironmentVariableGuard guard{ "PATH" };
    set_variable("PATH", "");
    ASSERT_TRUE(path().empty());
}

TEST(EnvironmentTest, find_global_real_binary)
{
    const std::filesystem::path& result{ find_dependency(REAL_BINARY, DependencySearchOption::Global) };
    ASSERT_FALSE(result.empty());
    ASSERT_TRUE(std::filesystem::exists(result));
    ASSERT_EQ(REAL_BINARY, result.filename().string());
}

TEST(EnvironmentTest, find_system_real_binary)
{
    const std::filesystem::path& result{ find_dependency(REAL_BINARY, DependencySearchOption::System) };
    ASSERT_FALSE(result.empty());
    ASSERT_TRUE(std::filesystem::exists(result));
    ASSERT_EQ(REAL_BINARY, result.filename().string());
}

TEST(EnvironmentTest, find_app_executable)
{
    std::string name{ executable_path().filename().string() };
    ASSERT_EQ(executable_path(), find_dependency(name, DependencySearchOption::App));
}

TEST(EnvironmentTest, find_global_executable)
{
    std::string name{ executable_path().filename().string() };
    ASSERT_EQ(executable_path(), find_dependency(name, DependencySearchOption::Global));
}

TEST(EnvironmentTest, find_app_ignores_path)
{
    ASSERT_TRUE(find_dependency(REAL_BINARY, DependencySearchOption::App).empty());
}

TEST(EnvironmentTest, find_local_ignores_path)
{
    ASSERT_TRUE(find_dependency(REAL_BINARY, DependencySearchOption::Local).empty());
}

TEST(EnvironmentTest, find_missing)
{
    ASSERT_TRUE(find_dependency("parabolic_missing_dependency", DependencySearchOption::Global).empty());
}

TEST(EnvironmentTest, find_cached)
{
    const std::filesystem::path& first{ find_dependency(REAL_BINARY, DependencySearchOption::Global) };
    const std::filesystem::path& second{ find_dependency(REAL_BINARY, DependencySearchOption::Global) };
    ASSERT_EQ(&first, &second);
}

TEST(EnvironmentTest, deployment_mode_local)
{
    ASSERT_EQ(DeploymentMode::Local, deployment_mode());
}

TEST(EnvironmentTest, locale_not_empty)
{
    ASSERT_FALSE(locale().empty());
}