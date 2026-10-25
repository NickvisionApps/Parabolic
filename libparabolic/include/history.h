#pragma once

#include <chrono>
#include <string>

namespace parabolic::history
{
    enum class Length
    {
        Never = 0,
        OneDay = 1,
        OneWeek = 7,
        OneMonth = 30,
        ThreeMonths = 90,
        SixMonths = 180,
        OneYear = 365,
        Forever = -1
    };

    class Entry
    {
    public:
        Entry(std::string url);
        [[nodiscard]] const std::string& url() const;
        [[nodiscard]] const std::string& title() const;
        void set_title(const std::string& title);
        [[nodiscard]] const std::string& path() const;
        void set_path(const std::string& path);
        [[nodiscard]] const std::chrono::system_clock::time_point& downloaded_on() const;
        void set_downloaded_on(const std::chrono::system_clock::time_point& downloaded_on);
        std::strong_ordering operator<=>(const Entry& rhs) const;
        bool operator==(const Entry& rhs) const;

    private:
        std::string m_url;
        std::string m_title;
        std::string m_path;
        std::chrono::system_clock::time_point m_downloaded_on;
    };
}