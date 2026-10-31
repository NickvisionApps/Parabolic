#include "environment.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <reproc++/reproc.hpp>
#include <sago/platform_folders.h>
#include <string>
#include <string_view>
#include <system_error>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>
#include "string_manip.h"
#ifdef _WIN32
#include <shellapi.h>
#include <windows.h>
#else
#include <csignal>
#include <cstdlib>
#include <unistd.h>
#ifdef __APPLE__
#include <CoreFoundation/CFBase.h>
#include <CoreFoundation/CFLocale.h>
#include <CoreFoundation/CFString.h>
#include <crt_externs.h>
#include <mach-o/dyld.h>
#include <sys/syslimits.h>
#else
#include <fstream>
#include <locale>
#include <sys/prctl.h>
#endif
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

namespace parabolic::environment
{
    std::expected<Process, std::error_code> Process::start(const std::vector<std::string>& command,
                                                           const reproc::options& options)
    {
        Process process;
#ifdef _WIN32
        std::error_code ec{ process.m_process.start(command, options) };
        if (ec)
        {
            return std::unexpected{ ec };
        }
        HANDLE job{ CreateJobObjectW(nullptr, nullptr) };
        if (job)
        {
            JOBOBJECT_EXTENDED_LIMIT_INFORMATION info{};
            info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
            SetInformationJobObject(job, JobObjectExtendedLimitInformation, &info, sizeof(info));
            HANDLE handle{ OpenProcess(PROCESS_SET_QUOTA | PROCESS_TERMINATE, FALSE,
                                       static_cast<DWORD>(process.m_process.pid().first)) };
            if (handle)
            {
                AssignProcessToJobObject(job, handle);
                CloseHandle(handle);
            }
            process.m_job = job;
        }
#else
        std::vector<char*> argv;
        argv.reserve(command.size() + 1);
        for (const std::string& arg : command)
        {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr);
        std::pair<bool, std::error_code> forked{ process.m_process.fork(options) };
        if (forked.second)
        {
            return std::unexpected{ forked.second };
        }
        if (forked.first)
        {
            setpgid(0, 0);
#ifdef __linux__
            prctl(PR_SET_PDEATHSIG, SIGKILL);
#endif
            execvp(argv[0], argv.data());
            _exit(127);
        }
        process.m_pid = process.m_process.pid().first;
#endif
        return process;
    }

    Process::Process(Process&& other) noexcept
        : m_process{ std::move(other.m_process) },
#ifdef _WIN32
          m_job{ std::exchange(other.m_job, nullptr) }
#else
          m_pid{ std::exchange(other.m_pid, 0) }
#endif
    {
    }

    Process::~Process()
    {
#ifdef _WIN32
        if (m_job)
        {
            CloseHandle(m_job);
        }
        else
        {
            std::ignore = m_process.kill();
        }
#else
        if (m_pid > 0)
        {
            kill();
            std::ignore = m_process.wait(reproc::infinite);
        }
#endif
    }

    void Process::kill()
    {
#ifdef _WIN32
        if (m_job)
        {
            TerminateJobObject(m_job, 1);
            return;
        }
        std::ignore = m_process.kill();
#else
        if (m_pid <= 0 || ::kill(-m_pid, SIGKILL) != 0)
        {
            std::ignore = m_process.kill();
        }
#endif
    }

    std::pair<int, std::error_code> Process::wait()
    {
        std::pair<int, std::error_code> status{ m_process.wait(reproc::infinite) };
#ifndef _WIN32
        if (!status.second)
        {
            m_pid = 0;
        }
#endif
        return status;
    }

    const std::vector<std::string>& arguments()
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
#elifdef __APPLE__
            int argc{ *_NSGetArgc() };
            char** argv{ *_NSGetArgv() };
            args.reserve(argc);
            for (int i = 0; i < argc; ++i)
            {
                args.emplace_back(argv[i]);
            }
#elifdef __linux__
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

    DeploymentMode deployment_mode()
    {
#ifdef _WIN32
        return DeploymentMode::Local;
#elifdef __APPLE__
        return DeploymentMode::Local;
#elifdef __linux__
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

    const std::filesystem::path& find_dependency(std::string_view name, DependencySearchOption option)
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
            for (const std::filesystem::path& dir : path())
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

    std::filesystem::path executable_directory()
    {
        return executable_path().parent_path();
    }

    std::filesystem::path executable_path()
    {
#ifdef _WIN32
        std::array<wchar_t, MAX_PATH> path{};
        DWORD len{ GetModuleFileNameW(nullptr, path.data(), MAX_PATH) };
        if (len > 0)
        {
            return std::filesystem::path{ std::wstring(path.data(), len) };
        }
        return {};
#elifdef __APPLE__
        std::array<char, PATH_MAX + 1> path{};
        uint32_t size{ static_cast<uint32_t>(path.size()) };
        if (_NSGetExecutablePath(path.data(), &size) == 0)
        {
            return std::filesystem::canonical(path.data());
        }
        return {};
#elifdef __linux__
        return std::filesystem::canonical("/proc/self/exe");
#else
        return {};
#endif
    }

    bool is_portable()
    {
        return std::ranges::find(arguments(), "--portable") != arguments().end();
    }

    std::string locale()
    {
#ifdef _WIN32
        LCID lcid{ GetThreadLocale() };
        std::array<wchar_t, LOCALE_NAME_MAX_LENGTH> name{};
        if (LCIDToLocaleName(lcid, name.data(), LOCALE_NAME_MAX_LENGTH, 0) > 0)
        {
            return string_manip::str(std::wstring(name.data()));
        }
        return {};
#elifdef __APPLE__
        std::string result;
        CFLocaleRef locale{ CFLocaleCopyCurrent() };
        if (locale != nullptr)
        {
            CFStringRef identifier{ CFLocaleGetIdentifier(locale) };
            if (identifier != nullptr)
            {
                std::array<char, 128> buffer{};
                if (CFStringGetCString(identifier, buffer.data(), buffer.size(), kCFStringEncodingUTF8) != 0)
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

    std::vector<std::filesystem::path> path()
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

    void set_variable(const std::string& name, const std::string& value)
    {
#ifdef _WIN32
        std::wstring wname{ string_manip::wstr(name) };
        std::wstring wvalue{ string_manip::wstr(value) };
        SetEnvironmentVariableW(wname.c_str(), wvalue.c_str());
#else
        setenv(name.c_str(), value.c_str(), 1);
#endif
    }

    std::string variable(const std::string& name)
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
        return env != nullptr ? std::string{ env } : std::string{};
#endif
    }
}
