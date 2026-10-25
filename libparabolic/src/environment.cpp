#include "environment.h"
#include <algorithm>
#include <array>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <locale>
#include <sago/platform_folders.h>
#include <unordered_map>
#include "string_manip.h"
#ifdef _WIN32
#include <shellapi.h>
#include <windows.h>
#elif defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#include <crt_externs.h>
#include <mach-o/dyld.h>
#endif

namespace
{
    bool search_in(const std::filesystem::path& dep, std::filesystem::path& result, const std::filesystem::path& dir)
    {
        std::filesystem::path candidate{ dir / dep };
        if (!std::filesystem::exists(candidate))
        {
            return false;
        }
        result = std::move(candidate);
        return true;
    }
}

namespace parabolic
{
    const std::vector<std::string>& environment::arguments()
    {
        static std::vector<std::string> args;
        if (args.empty())
        {
#ifdef _WIN32
            int argc{ 0 };
            if (LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc))
            {
                args.reserve(argc);
                for (int i = 0; i < argc; ++i)
                {
                    args.emplace_back(string_manip::str({ argv[i] }));
                }
                LocalFree(argv);
            }
#elif defined(__APPLE__)
            int argc{ *_NSGetArgc() };
            char** argv{ *_NSGetArgv() };
            args.reserve(argc);
            for (int i = 0; i < argc; ++i)
            {
                args.emplace_back(argv[i]);
            }
#elif defined(__linux__)
            std::ifstream file("/proc/self/cmdline", std::ios::binary);
            std::string arg;
            while (std::getline(file, arg, '\0'))
            {
                args.emplace_back(arg);
            }
#endif
        }
        return args;
    }

    environment::DeploymentMode environment::deployment_mode()
    {
#ifdef _WIN32
        return DeploymentMode::Local;
#elif defined(__APPLE__)
        return DeploymentMode::Local;
#elif defined(__linux__)
        if (std::filesystem::exists("/.flatpak-info"))
        {
            return DeploymentMode::Flatpak;
        }
        else if (!variable("SNAP").empty())
        {
            return DeploymentMode::Snap;
        }
        else if (!variable("APPIMAGE").empty())
        {
            return DeploymentMode::AppImage;
        }
        else if (std::filesystem::exists("/proc/sys/fs/binfmt_misc/WSLInterop"))
        {
            return DeploymentMode::WSL;
        }
        else
        {
            return DeploymentMode::Local;
        }
#else
        return DeploymentMode::Local;
#endif
    }

    const std::filesystem::path& environment::find_dependency(std::string_view name, DependencySearchOption option)
    {
        static std::unordered_map<std::string, std::filesystem::path> dependencies;
        std::filesystem::path dep{ name };
        std::string key{ dep.string() + "|" + std::to_string(static_cast<int>(option)) };
        auto it{ dependencies.find(key) };
        if (it != dependencies.end() && std::filesystem::exists(it->second))
        {
            return it->second;
        }
        dependencies[key] = std::filesystem::path();
        std::filesystem::path& result{ dependencies[key] };
        if (option == DependencySearchOption::Global || option == DependencySearchOption::App)
        {
            search_in(dep, result, executable_directory());
        }
        if (result.empty() && (option == DependencySearchOption::Global || option == DependencySearchOption::System))
        {
            for (const std::filesystem::path& dir: path())
            {
                if (search_in(dep, result, dir))
                {
                    break;
                }
            }
        }
        if (result.empty() && option == DependencySearchOption::Local)
        {
            search_in(dep, result, sago::getDataHome());
        }
        return result;
    }

    std::filesystem::path environment::executable_directory()
    {
        return executable_path().parent_path();
    }

    std::filesystem::path environment::executable_path()
    {
#ifdef _WIN32
        std::array<wchar_t, MAX_PATH> path{};
        DWORD len{ GetModuleFileNameW(nullptr, path.data(), MAX_PATH) };
        if (len > 0)
        {
            return std::filesystem::path{ std::wstring(path.data(), len) };
        }
        return {};
#elif defined(__APPLE__)
        std::array<char, PATH_MAX + 1> path{};
        uint32_t size{ static_cast<uint32_t>(path.size()) };
        if (_NSGetExecutablePath(path.data(), &size) == 0)
        {
            return std::filesystem::canonical(path.data());
        }
        return {};
#elif defined(__linux__)
        return std::filesystem::canonical("/proc/self/exe");
#else
        return {};
#endif
    }

    bool environment::is_portable()
    {
        return std::ranges::find(arguments(), "--portable") != arguments().end();
    }

    std::string environment::locale()
    {
#ifdef _WIN32
        LCID lcid{ GetThreadLocale() };
        std::array<wchar_t, LOCALE_NAME_MAX_LENGTH> name{};
        if (LCIDToLocaleName(lcid, name.data(), LOCALE_NAME_MAX_LENGTH, 0) > 0)
        {
            return string_manip::str(std::wstring(name.data()));
        }
        return {};
#elif defined(__APPLE__)
        std::string result;
        CFLocaleRef locale{ CFLocaleCopyCurrent() };
        if (locale)
        {
            CFStringRef identifier{ CFLocaleGetIdentifier(locale) };
            if (identifier)
            {
                std::array<char, 128> buffer{};
                if (CFStringGetCString(identifier, buffer.data(), buffer.size(), kCFStringEncodingUTF8))
                {
                    result = buffer.data();
                }
            }
            CFRelease(locale);
        }
        return result;
#else
        try
        {
            return std::locale("").name();
        }
        catch (...)
        {
            return {};
        }
#endif
    }

    std::vector<std::filesystem::path> environment::path()
    {
        std::string env{ variable("PATH") };
        if (!env.empty())
        {
#ifdef _WIN32
            return string_manip::split<std::filesystem::path>(env, ';', false);
#else
            return string_manip::split<std::filesystem::path>(env, ':', false);
#endif
        }
        return {};
    }

    void environment::set_variable(const std::string& name, const std::string& value)
    {
#ifdef _WIN32
        std::wstring wname{ string_manip::wstr(name) };
        std::wstring wvalue{ string_manip::wstr(value) };
        SetEnvironmentVariableW(wname.c_str(), wvalue.c_str());
#else
        setenv(name.c_str(), value.c_str(), 1);
#endif
    }

    std::string environment::variable(const std::string& name)
    {
#ifdef _WIN32
        std::wstring wname{ string_manip::wstr(name) };
        std::wstring wvalue;
        DWORD size{ GetEnvironmentVariableW(wname.c_str(), nullptr, 0) };
        while (size > 0)
        {
            wvalue.resize(size);
            DWORD len{ GetEnvironmentVariableW(wname.c_str(), wvalue.data(), size) };
            if (len == 0)
            {
                wvalue.clear();
                break;
            }
            if (len < size)
            {
                wvalue.resize(len);
                break;
            }
            size = len;
        }
        return string_manip::str(wvalue);
#else
        const char* env{ getenv(name.c_str()) };
        return env ? std::string{ env } : std::string{};
#endif
    }
}
