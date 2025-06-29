#include "models/historicdownload.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    HistoricDownload::HistoricDownload(const std::string& url)
        : m_url{ url },
        m_dateTime{ boost::posix_time::second_clock::universal_time() }
    {

    }

    HistoricDownload::HistoricDownload(const std::string& url, const std::string& title, const std::filesystem::path& path)
        : m_url{ url },
        m_title{ title },
        m_path{ path },
        m_dateTime{ boost::posix_time::second_clock::universal_time() }
    {

    }

    HistoricDownload::HistoricDownload(const std::string& url, const std::string& title, const std::filesystem::path& path, const boost::posix_time::ptime& dateTime)
        : m_url{ url },
        m_title{ title },
        m_path{ path },
        m_dateTime{ dateTime }
    {

    }

    const std::string& HistoricDownload::getUrl() const
    {
        return m_url;
    }

    void HistoricDownload::setUrl(const std::string& url)
    {
        m_url = url;
    }

    const std::string& HistoricDownload::getTitle() const
    {
        return m_title;
    }

    void HistoricDownload::setTitle(const std::string& title)
    {
        m_title = title;
    }

    const std::filesystem::path& HistoricDownload::getPath() const
    {
        return m_path;
    }

    void HistoricDownload::setPath(const std::filesystem::path& path)
    {
        m_path = path;
    }

    const boost::posix_time::ptime& HistoricDownload::getDateTime() const
    {
        return m_dateTime;
    }

    void HistoricDownload::setDateTime(const boost::posix_time::ptime& dateTime)
    {
        m_dateTime = dateTime;
    }

    bool HistoricDownload::operator==(const HistoricDownload& other) const
    {
        return m_url == other.m_url;
    }

    bool HistoricDownload::operator!=(const HistoricDownload& other) const
    {
        return !operator==(other);
    }

    bool HistoricDownload::operator<(const HistoricDownload& other) const
    {
        return m_dateTime < other.m_dateTime;
    }

    bool HistoricDownload::operator>(const HistoricDownload& other) const
    {
        return m_dateTime > other.m_dateTime;
    }
}
