#include "models/historicdownload.h"
#include <boost/date_time/gregorian/gregorian.hpp>

namespace Nickvision::TubeConverter::Shared::Models
{
    HistoricDownload::HistoricDownload(const std::string& url)
        : m_url{ url },
        m_dateTime{ boost::gregorian::day_clock::universal_day(), boost::posix_time::second_clock::universal_time().time_of_day() }
    {

    }

    HistoricDownload::HistoricDownload(const std::string& url, const std::string& title, const std::filesystem::path& path)
        : m_url{ url },
        m_title{ title },
        m_path{ path },
        m_dateTime{ boost::gregorian::day_clock::universal_day(), boost::posix_time::second_clock::universal_time().time_of_day() }
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
        if(std::filesystem::exists(m_path))
        {
            return m_path;
        }
        static std::filesystem::path emptyPath{};
        return emptyPath;
    }

    void HistoricDownload::setPath(const std::filesystem::path& path)
    {
        if(std::filesystem::exists(path))
        {
            m_path = path;
        }
        else
        {
            m_path = "";
        }
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
}