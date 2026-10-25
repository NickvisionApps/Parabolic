#include "history.h"

#include <SQLiteCpp/SQLiteCpp.h>
#include <algorithm>
#include <chrono>
#include <compare>
#include <functional>
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
    Entry::Entry(std::string url)
        : m_url{ std::move(url) }
    {
    }

    const std::string& Entry::url() const
    {
        return m_url;
    }

    const std::string& Entry::title() const
    {
        return m_title;
    }

    void Entry::set_title(const std::string& title)
    {
        m_title = title;
    }

    const std::string& Entry::path() const
    {
        return m_path;
    }

    void Entry::set_path(const std::string& path)
    {
        m_path = path;
    }

    const std::chrono::system_clock::time_point& Entry::downloaded_on() const
    {
        return m_downloaded_on;
    }

    void Entry::set_downloaded_on(const std::chrono::system_clock::time_point& downloaded_on)
    {
        m_downloaded_on = downloaded_on;
    }

    std::strong_ordering Entry::operator<=>(const Entry& rhs) const
    {
        return m_downloaded_on <=> rhs.m_downloaded_on;
    }

    bool Entry::operator==(const Entry& rhs) const
    {
        return m_url == rhs.m_url;
    }

    Manager::Manager(std::string_view app_name)
#ifdef _WIN32
        : m_database{ environment::is_portable() ? environment::executable_directory() / "history.db"
                                                 : std::filesystem::path(string_manip::wstr(sago::getConfigHome())) /
                                                           app_name / "history.db",
                      SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE }
#else
        : m_database{ environment::is_portable()
                              ? environment::executable_directory() / "history.db"
                              : std::filesystem::path(sago::getConfigHome()) / app_name / "history.db",
                      SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE }
#endif
    {
        m_database.exec("CREATE TABLE IF NOT EXISTS " TABLE_NAME
                        " (url TEXT PRIMARY KEY, title TEXT, path TEXT, downloaded_on INTEGER)");
    }

    Length Manager::length() const
    {
        return m_length;
    }

    void Manager::set_length(Length length)
    {
        m_length = length;
    }

    bool Manager::sort_newest() const
    {
        return m_sort_newest;
    }

    void Manager::set_sort_newest(bool sort_newest)
    {
        m_sort_newest = sort_newest;
    }

    bool Manager::add_or_update(const Entry& entry)
    {
        if (m_length == Length::Never)
        {
            return true;
        }
        SQLite::Statement query{ m_database, "INSERT OR REPLACE INTO " TABLE_NAME
                                             " (url, title, path, downloaded_on) VALUES (?, ?, ?, ?)" };
        query.bind(1, entry.url());
        query.bind(2, entry.title());
        query.bind(3, entry.path());
        query.bind(4,
                   std::chrono::duration_cast<std::chrono::seconds>(entry.downloaded_on().time_since_epoch()).count());
        return query.exec() > 0;
    }

    bool Manager::add_or_update(const std::vector<Entry>& entries)
    {
        SQLite::Transaction transaction{ m_database };
        bool success{ true };
        for (const Entry& entry: entries)
        {
            success &= add_or_update(entry);
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

    bool Manager::clear()
    {
        SQLite::Statement query{ m_database, "DELETE FROM " TABLE_NAME };
        return query.exec() > 0;
    }

    std::vector<Entry> Manager::entries()
    {
        std::vector<Entry> result;
        std::vector<std::string> marked;
        SQLite::Statement query{ m_database, "SELECT url, title, path, downloaded_on FROM " TABLE_NAME };
        while (query.executeStep())
        {
            Entry entry{ query.getColumn(0).getString() };
            entry.set_title(query.getColumn(1).getString());
            entry.set_path(query.getColumn(2).getString());
            entry.set_downloaded_on(
                    std::chrono::system_clock::time_point{ std::chrono::seconds{ query.getColumn(3).getInt64() } });
            if (m_length != Length::Forever &&
                std::chrono::duration_cast<std::chrono::hours>(std::chrono::system_clock::now() - entry.downloaded_on())
                                        .count() /
                                24 >
                        static_cast<int>(m_length))
            {
                marked.push_back(entry.url());
                continue;
            }
            result.push_back(entry);
        }
        if (!marked.empty())
        {
            SQLite::Transaction transaction{ m_database };
            bool success{ true };
            for (const std::string& url: marked)
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
        if (m_sort_newest)
        {
            std::ranges::sort(result, std::ranges::greater{});
        }
        else
        {
            std::ranges::sort(result, std::ranges::less{});
        }
        return result;
    }

    bool Manager::remove(const std::string& url)
    {
        SQLite::Statement query{ m_database, "DELETE FROM " TABLE_NAME " WHERE url = ?" };
        query.bind(1, url);
        return query.exec() > 0;
    }
}
