#include "models/downloadhistory.h"
#include <algorithm>

using namespace Nickvision::Helpers;

namespace Nickvision::TubeConverter::Shared::Models
{
    DownloadHistory::DownloadHistory(const std::filesystem::path& path)
        : JsonFileBase{ path },
        m_length{ static_cast<HistoryLength>(get("Length", static_cast<int>(HistoryLength::OneWeek))) }
    {
        boost::json::array jsonHistory = get<boost::json::array>("History", {});
        if(m_length != HistoryLength::Never && jsonHistory.size() > 0)
        {
            m_history.reserve(jsonHistory.size());
            for(const boost::json::value& value : jsonHistory)
            {
                if(!value.is_object())
                {
                    continue;
                }
                boost::json::object history = value.as_object();
                HistoricDownload download{ history["URL"].is_string() ? history["URL"].as_string().c_str() : "" };
                if(download.getUrl().empty())
                {
                    continue;
                }
                download.setTitle(history["Title"].is_string() ? history["Title"].as_string().c_str() : "");
                download.setPath(history["Path"].is_string() ? history["Path"].as_string().c_str() : "");
                try
                {
                    download.setDateTime(boost::posix_time::from_iso_string(history["DateTime"].is_string() ? history["DateTime"].as_string().c_str() : ""));
                }
                catch(...) { }
                if(m_length != HistoryLength::Forever)
                {
                    boost::gregorian::days daysSinceDownload{ boost::posix_time::second_clock::universal_time().date() - download.getDateTime().date() };
                    if(daysSinceDownload > boost::gregorian::days{ static_cast<int>(m_length) })
                    {
                        continue;
                    }
                }
                m_history.push_back(download);
            }
        }
        updateDisk();
    }

    const std::vector<HistoricDownload>& DownloadHistory::getHistory() const
    {
        return m_history;
    }

    HistoryLength DownloadHistory::getLength() const
    {
        return m_length;
    }

    void DownloadHistory::setLength(HistoryLength length)
    {
        if(m_length == length)
        {
            return;
        }
        m_length = length;
        if(m_length == HistoryLength::Never)
        {
            m_history.clear();
        }
        else if(m_length != HistoryLength::Forever)
        {
            for(std::vector<HistoricDownload>::iterator it = m_history.begin(); it != m_history.end();)
            {
                boost::gregorian::days daysSinceDownload{ boost::posix_time::second_clock::universal_time().date() - it->getDateTime().date() };
                if(daysSinceDownload > boost::gregorian::days{ static_cast<int>(m_length) })
                {
                    it = m_history.erase(it);
                    continue;
                }
                ++it;
            }
        }
        updateDisk();
    }

    bool DownloadHistory::addDownload(const HistoricDownload& download)
    {
        if(m_length == HistoryLength::Never)
        {
            return false;
        }
        boost::gregorian::days daysSinceDownload{ boost::posix_time::second_clock::universal_time().date() - download.getDateTime().date() };
        if(m_length != HistoryLength::Forever && daysSinceDownload > boost::gregorian::days{ static_cast<int>(m_length) })
        {
            return false;
        }
        else if(std::find(m_history.begin(), m_history.end(), download) != m_history.end())
        {
            return false;
        }
        m_history.push_back(download);
        updateDisk();
        return true;
    }

    bool DownloadHistory::updateDownload(const HistoricDownload& download)
    {
        if(m_length == HistoryLength::Never)
        {
            return false;
        }
        std::vector<HistoricDownload>::iterator it{ std::find(m_history.begin(), m_history.end(), download) };
        boost::gregorian::days daysSinceDownload{ boost::posix_time::second_clock::universal_time().date() - download.getDateTime().date() };
        if(it == m_history.end())
        {
            return false;
        }
        else if(m_length != HistoryLength::Forever && daysSinceDownload > boost::gregorian::days{ static_cast<int>(m_length) })
        {
            return false;
        }
        *it = download;
        updateDisk();
        return true;
    }

    bool DownloadHistory::removeDownload(const HistoricDownload& download)
    {
        if(m_length == HistoryLength::Never)
        {
            return false;
        }
        std::vector<HistoricDownload>::iterator it{ std::find(m_history.begin(), m_history.end(), download) };
        if(it == m_history.end())
        {
            return false;
        }
        m_history.erase(it);
        updateDisk();
        return true;
    }

    bool DownloadHistory::clear()
    {
        m_history.clear();
        set("Length", static_cast<int>(HistoryLength::OneWeek));
        set<boost::json::array>("History", {});
        save();
        return true;
    }

    void DownloadHistory::updateDisk()
    {
        std::sort(m_history.begin(), m_history.end());
        set("Length", static_cast<int>(m_length));
        boost::json::array arr;
        for(const HistoricDownload& download : m_history)
        {
            boost::json::object obj;
            obj["URL"] = download.getUrl();
            obj["Title"] = download.getTitle();
            obj["Path"] = download.getPath().string();
            obj["DateTime"] = boost::posix_time::to_iso_string(download.getDateTime());
            arr.push_back(obj);
        }
        set("History", arr);
        save();
    }
}
