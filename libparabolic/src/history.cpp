#include "history.h"

namespace parabolic
{
    HistoricDownload::HistoricDownload(std::string url)
        : m_url{ std::move(url) },
        m_downloaded_on{ std::chrono::system_clock::now() }
    {
    }

    const std::string& HistoricDownload::url() const
    {
        return m_url;
    }

    const std::string& HistoricDownload::title() const
    {
        return m_title;
    }

    void HistoricDownload::set_title(const std::string& title)
    {
        m_title = title;
    }

    const std::string& HistoricDownload::path() const
    {
        return m_path;
    }

    void HistoricDownload::set_path(const std::string& path)
    {
        m_path = path;
    }

    const std::chrono::system_clock::time_point& HistoricDownload::downloaded_on() const
    {
        return m_downloaded_on;
    }

    void HistoricDownload::set_downloaded_on(const std::chrono::system_clock::time_point& downloaded_on)
    {
        m_downloaded_on = downloaded_on;
    }

    std::strong_ordering HistoricDownload::operator<=>(const HistoricDownload& rhs) const
    {
        return m_downloaded_on <=> rhs.m_downloaded_on;
    }

    bool HistoricDownload::operator==(const HistoricDownload& rhs) const
    {
        return m_url == rhs.m_url;
    }
}