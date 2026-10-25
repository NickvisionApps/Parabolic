#include "history.h"
#include <chrono>
#include <compare>
#include <string>
#include <utility>

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
}
