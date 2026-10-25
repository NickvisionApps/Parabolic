#include "string_manip.h"
#include <algorithm>
#include <cctype>
#include <cwchar>
#include <ranges>
#include <utility>
#ifdef _WIN32
#include <windows.h>
#endif

namespace parabolic::string_manip
{
    std::string lower(const std::string& str)
    {
        std::string result{ str };
        std::ranges::transform(result, result.begin(), [](unsigned char c)
        {
            return c >= 'A' && c <= 'Z' ? static_cast<char>(c + ('a' - 'A')) : static_cast<char>(c);
        });
        return result;
    }

    std::string trim(const std::string& str)
    {
        if (str.empty())
        {
            return str;
        }
        std::string result{ str };
        result.erase(std::ranges::find_if(std::views::reverse(result),
                                          [](unsigned char ch)
        {
            return !(ch == ' ' || (ch >= '\t' && ch <= '\r'));
        }).base(),
                     result.end());
        result.erase(result.begin(), std::ranges::find_if(result, [](unsigned char ch)
        {
            return !(ch == ' ' || (ch >= '\t' && ch <= '\r'));
        }));
        return result;
    }

    std::string str(const std::wstring& wstr)
    {
#ifdef _WIN32
        int size{ WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr) };
        if (size <= 0)
        {
            return {};
        }
        std::string result(size - 1, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, result.data(), size, nullptr, nullptr);
        return result;
#else
        std::mbstate_t state{};
        const wchar_t* ptr{ wstr.data() };
        std::size_t len{ std::wcsrtombs(nullptr, &ptr, 0, &state) };
        if (len == static_cast<std::size_t>(-1))
        {
            return {};
        }
        std::string result(len, '\0');
        state = std::mbstate_t{};
        std::wcsrtombs(result.data(), &ptr, len, &state);
        return result;
#endif
    }

    std::wstring wstr(const std::string& str)
    {
#ifdef _WIN32
        int size{ MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0) };
        if (size <= 0)
        {
            return {};
        }
        std::wstring result(size - 1, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, result.data(), size);
        return result;
#else
        std::mbstate_t state{};
        const char* ptr{ str.c_str() };
        std::size_t len{ std::mbsrtowcs(nullptr, &ptr, 0, &state) };
        if (len == static_cast<std::size_t>(-1))
        {
            return {};
        }
        std::wstring result(len, L'\0');
        ptr = str.c_str();
        state = std::mbstate_t{};
        std::mbsrtowcs(result.data(), &ptr, len, &state);
        return result;
#endif
    }
}
