#pragma once

#include <string>
#include <type_traits>
#include <vector>

namespace parabolic::string_manip
{
    template<typename T>
    concept ImplicitlyConstructableString =
            std::is_constructible_v<T, std::string> && std::is_convertible_v<std::string, T>;

    [[nodiscard]] std::string lower(const std::string& str);

    [[nodiscard]] std::string trim(const std::string& str);

    [[nodiscard]] std::string str(const std::wstring& wstr);

    [[nodiscard]] std::wstring wstr(const std::string& str);

    template<ImplicitlyConstructableString T = std::string>
    [[nodiscard]] std::vector<T> split(const std::string& s, const std::string& delimiter, bool include_empty = true)
    {
        std::vector<T> splits;
        size_t last{ 0 };
        size_t next{ 0 };
        while ((next = s.find(delimiter, last)) != std::string::npos)
        {
            std::string token{ s.substr(last, next - last) };
            if (include_empty || !trim(token).empty())
            {
                splits.push_back(std::move(token));
            }
            last = next + delimiter.length();
        }
        std::string final_token{ s.substr(last) };
        if (include_empty || !trim(final_token).empty())
        {
            splits.push_back(final_token);
        }
        return splits;
    }

    template<ImplicitlyConstructableString T = std::string>
    [[nodiscard]] std::vector<T> split(const std::string& s, char delimiter, bool include_empty = true) noexcept
    {
        return split<T>(s, std::string(1, delimiter), include_empty);
    }
}
