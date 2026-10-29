#ifndef HISTORY_H
#define HISTORY_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace parabolic::history
{
    enum class Length : std::int16_t
    {
        Never = 0,
        OneDay = 1,
        OneWeek = 7,
        OneMonth = 30,
        ThreeMonths = 90,
        SixMonths = 180,
        OneYear = 365,
        Forever = -1,
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
        std::chrono::system_clock::time_point m_downloaded_on{ std::chrono::system_clock::now() };
    };

    class Options
    {
    public:
        [[nodiscard]] Length length() const;
        void set_length(Length value);
        [[nodiscard]] bool sort_newest() const;
        void set_sort_newest(bool value);
        bool operator==(const Options& rhs) const = default;
        friend void from_json(const nlohmann::json& json, Options& options);
        friend void to_json(nlohmann::json& json, const Options& options);

    private:
        Length m_length{ Length::OneWeek };
        bool m_sort_newest{ true };
    };

    class Store
    {
    public:
        Store(std::string_view app_name);
        [[nodiscard]] bool add_or_update(const Entry& entry);
        [[nodiscard]] bool add_or_update(const std::vector<Entry>& entries);
        [[nodiscard]] bool clear();
        [[nodiscard]] std::vector<Entry> get_all();
        [[nodiscard]] const Options& options() const;
        [[nodiscard]] bool remove(const std::string& url);
        void set_options(const Options& options);

    private:
        SQLite::Database m_database;
        Options m_options;
    };
}

#endif
