#include "history.h"

#include <SQLiteCpp/SQLiteCpp.h>
#include <algorithm>
#include <chrono>
#include <compare>
#include <functional>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <sago/platform_folders.h>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "environment.h"
#ifdef _WIN32
#include "string_manip.h"
#endif

#define TABLE_NAME "history_cpp"

namespace parabolic::history
{
    Record::Record(std::string url)
        : m_url{ std::move(url) }
    {
    }

    const std::string& Record::url() const
    {
        return m_url;
    }

    const std::string& Record::title() const
    {
        return m_title;
    }

    void Record::set_title(const std::string& title)
    {
        m_title = title;
    }

    const std::string& Record::path() const
    {
        return m_path;
    }

    void Record::set_path(const std::string& path)
    {
        m_path = path;
    }

    const std::chrono::system_clock::time_point& Record::downloaded_on() const
    {
        return m_downloaded_on;
    }

    void Record::set_downloaded_on(const std::chrono::system_clock::time_point& downloaded_on)
    {
        m_downloaded_on = downloaded_on;
    }

    std::strong_ordering Record::operator<=>(const Record& rhs) const
    {
        return m_downloaded_on <=> rhs.m_downloaded_on;
    }

    bool Record::operator==(const Record& rhs) const
    {
        return m_url == rhs.m_url;
    }

    Length Options::length() const
    {
        return m_length;
    }

    void Options::set_length(Length value)
    {
        m_length = value;
    }

    bool Options::sort_newest() const
    {
        return m_sort_newest;
    }

    void Options::set_sort_newest(bool value)
    {
        m_sort_newest = value;
    }

    Store::Store(std::string_view app_name)
#ifdef _WIN32
        : m_database{ environment::is_portable() ? environment::executable_directory() / "history.db"
                                                 : std::filesystem::path(string_manip::wstr(sago::getConfigHome())) /
                                                           app_name / "history.db",
                      SQLite::OPEN_READWRITE + SQLite::OPEN_CREATE }
#else
        : m_database{ environment::is_portable()
                              ? environment::executable_directory() / "history.db"
                              : std::filesystem::path(sago::getConfigHome()) / app_name / "history.db",
                      SQLite::OPEN_READWRITE + SQLite::OPEN_CREATE }
#endif
    {
        m_database.exec("CREATE TABLE IF NOT EXISTS " TABLE_NAME
                        " (url TEXT PRIMARY KEY, title TEXT, path TEXT, downloaded_on INTEGER)");
    }

    bool Store::add_or_update(const Record& record)
    {
        if (m_options.length() == Length::Never)
        {
            return true;
        }
        SQLite::Statement query{ m_database, "INSERT OR REPLACE INTO " TABLE_NAME
                                             " (url, title, path, downloaded_on) VALUES (?, ?, ?, ?)" };
        query.bind(1, record.url());
        query.bind(2, record.title());
        query.bind(3, record.path());
        query.bind(4,
                   std::chrono::duration_cast<std::chrono::seconds>(record.downloaded_on().time_since_epoch()).count());
        return query.exec() > 0;
    }

    bool Store::add_or_update(const std::vector<Record>& records)
    {
        SQLite::Transaction transaction{ m_database };
        bool success{ true };
        for (const Record& record : records)
        {
            success &= add_or_update(record);
        }
        if (!success)
        {
            transaction.rollback();
        }
        else
        {
            transaction.commit();
        }
        return success;
    }

    bool Store::clear()
    {
        SQLite::Statement query{ m_database, "DELETE FROM " TABLE_NAME };
        return query.exec() > 0;
    }

    std::vector<Record> Store::get_all()
    {
        std::vector<Record> result;
        std::vector<std::string> marked;
        SQLite::Statement query{ m_database, "SELECT url, title, path, downloaded_on FROM " TABLE_NAME };
        while (query.executeStep())
        {
            Record record{ query.getColumn(0).getString() };
            record.set_title(query.getColumn(1).getString());
            record.set_path(query.getColumn(2).getString());
            record.set_downloaded_on(
                    std::chrono::system_clock::time_point{ std::chrono::seconds{ query.getColumn(3).getInt64() } });
            if (m_options.length() != Length::Forever &&
                std::chrono::duration_cast<std::chrono::hours>(std::chrono::system_clock::now() -
                                                               record.downloaded_on())
                                        .count() /
                                24 >
                        static_cast<int>(m_options.length()))
            {
                marked.push_back(record.url());
                continue;
            }
            result.push_back(record);
        }
        if (!marked.empty())
        {
            SQLite::Transaction transaction{ m_database };
            bool success{ true };
            for (const std::string& url : marked)
            {
                success &= remove(url);
            }
            if (!success)
            {
                transaction.rollback();
            }
            else
            {
                transaction.commit();
            }
        }
        if (m_options.sort_newest())
        {
            std::ranges::sort(result, std::ranges::greater{});
        }
        else
        {
            std::ranges::sort(result, std::ranges::less{});
        }
        return result;
    }

    bool Store::remove(const std::string& url)
    {
        SQLite::Statement query{ m_database, "DELETE FROM " TABLE_NAME " WHERE url = ?" };
        query.bind(1, url);
        return query.exec() > 0;
    }

    void Store::set_options(const Options& options)
    {
        m_options = options;
    }

    void from_json(const nlohmann::json& json, Options& options)
    {
        if (json.contains("historyLength") && json["historyLength"].is_number_integer())
        {
            int length{ json.at("historyLength").get<int>() };
            switch (length)
            {
            case static_cast<int>(Length::Never):
            case static_cast<int>(Length::OneDay):
            case static_cast<int>(Length::OneWeek):
            case static_cast<int>(Length::OneMonth):
            case static_cast<int>(Length::ThreeMonths):
            case static_cast<int>(Length::SixMonths):
            case static_cast<int>(Length::OneYear):
            case static_cast<int>(Length::Forever):
                options.m_length = static_cast<Length>(length);
                break;
            default:
                break;
            }
        }
        else if (json.contains("HistoryLength") && json["HistoryLength"].is_number_integer())
        {
            int length{ json.at("HistoryLength").get<int>() };
            switch (length)
            {
            case static_cast<int>(Length::Never):
            case static_cast<int>(Length::OneDay):
            case static_cast<int>(Length::OneWeek):
            case static_cast<int>(Length::OneMonth):
            case static_cast<int>(Length::ThreeMonths):
            case static_cast<int>(Length::SixMonths):
            case static_cast<int>(Length::OneYear):
            case static_cast<int>(Length::Forever):
                options.m_length = static_cast<Length>(length);
                break;
            default:
                break;
            }
        }
        if (json.contains("historySortNewestFirst") && json["historySortNewestFirst"].is_boolean())
        {
            options.m_sort_newest = json.at("historySortNewestFirst").get<bool>();
        }
        else if (json.contains("HistorySortNewestFirst") && json["HistorySortNewestFirst"].is_boolean())
        {
            options.m_sort_newest = json.at("HistorySortNewestFirst").get<bool>();
        }
    }

    void to_json(nlohmann::json& json, const Options& options)
    {
        json = nlohmann::json{
            { "historyLength", static_cast<int>(options.m_length) },
            { "historySortNewestFirst", options.m_sort_newest },
        };
    }
}
