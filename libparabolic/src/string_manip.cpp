#include "string_manip.h"
#include <algorithm>
#include <cctype>
#include <cwchar>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#ifdef _WIN32
#include <windows.h>
#endif

namespace parabolic::string_manip
{
    std::string lower(std::string_view str)
    {
        std::string result{ str };
        std::ranges::transform(result, result.begin(), [](unsigned char c)
        {
            return c >= 'A' && c <= 'Z' ? static_cast<char>(c + ('a' - 'A')) : static_cast<char>(c);
        });
        return result;
    }

    std::string sanitize_for_filename(std::string_view str, bool include_windows_characters)
    {
        std::string result{ str };
#ifdef _WIN32
        include_windows_characters = true;
#endif
        std::ranges::replace_if(result, [include_windows_characters](char c)
        {
            return c == '\0' || c == '/' ||
                   (include_windows_characters &&
                    (static_cast<unsigned char>(c) < 32 || std::string_view{ "<>:\"\\|?*" }.contains(c)));
        }, '_');
        return result;
    }

    std::string trim(std::string_view str)
    {
        if (str.empty())
        {
            return {};
        }
        std::string result{ str };
        result.erase(std::ranges::find_if(std::views::reverse(result),
                                          [](unsigned char ch)
        {
            return ch != ' ' && (ch < '\t' || ch > '\r');
        }).base(),
                     result.end());
        result.erase(result.begin(), std::ranges::find_if(result, [](unsigned char ch)
        {
            return ch != ' ' && (ch < '\t' || ch > '\r');
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
        if (std::cmp_equal(len, -1))
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
        if (std::cmp_equal(len, -1))
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
