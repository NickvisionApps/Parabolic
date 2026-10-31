#include "string_manip.h"
#include <algorithm>
#include <cctype>
#include <cwchar>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif

namespace parabolic::string_manip
{
    std::vector<std::string> command_line_split(const std::string& str)
    {
        if (str.empty())
        {
            return {};
        }
        std::vector<std::string> args;
        std::string current;
        bool in_quotes{ false };
        bool has_token{ false };
        for (char c : str)
        {
            if (c == '"')
            {
                in_quotes = !in_quotes;
                has_token = true;
            }
            else if (!in_quotes && (c == ' ' || (c >= '\t' && c <= '\r')))
            {
                if (has_token)
                {
                    args.push_back(std::move(current));
                    current.clear();
                    has_token = false;
                }
            }
            else
            {
                current += c;
                has_token = true;
            }
        }
        if (has_token)
        {
            args.push_back(std::move(current));
        }
        return args;
    }

    std::string lower(std::string str)
    {
        std::ranges::transform(str, str.begin(), [](unsigned char c)
        {
            return c >= 'A' && c <= 'Z' ? static_cast<char>(c + ('a' - 'A')) : static_cast<char>(c);
        });
        return str;
    }

    std::string sanitize_for_filename(std::string str, bool include_windows_characters)
    {
#ifdef _WIN32
        include_windows_characters = true;
#endif
        std::ranges::replace_if(str, [include_windows_characters](char c)
        {
            return c == '\0' || c == '/' ||
                   (include_windows_characters &&
                    (static_cast<unsigned char>(c) < 32 || std::string_view{ "<>:\"\\|?*" }.contains(c)));
        }, '_');
        return str;
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

    std::string trim(std::string str)
    {
        if (str.empty())
        {
            return {};
        }
        str.erase(std::ranges::find_if(std::views::reverse(str),
                                       [](unsigned char ch)
        {
            return ch != ' ' && (ch < '\t' || ch > '\r');
        }).base(),
                  str.end());
        str.erase(str.begin(), std::ranges::find_if(str, [](unsigned char ch)
        {
            return ch != ' ' && (ch < '\t' || ch > '\r');
        }));
        return str;
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
