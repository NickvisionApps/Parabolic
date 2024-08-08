#include "models/downloadhistory.h"
#include <algorithm>

using namespace Nickvision::App;

namespace Nickvision::TubeConverter::Shared::Models
{
    DownloadHistory::DownloadHistory(const std::string& key, const std::string& appName)
        : DataFileBase{ key, appName },
        m_length{ static_cast<HistoryLength>(m_json.get("Length", static_cast<int>(HistoryLength::OneWeek)).asInt()) }
    {
        if(m_length != HistoryLength::Never)
        {
            const Json::Value historyJson{ m_json["History"] };
            for(unsigned int i = 0; i < historyJson.size(); i++)
            {
                HistoricDownload download{ historyJson[i].get("URL", "").asString() };
                if(download.getUrl().empty())
                {
                    continue;
                }
                download.setTitle(historyJson[i].get("Title", "").asString());
                download.setPath(historyJson[i].get("Path", "").asString());
                try
                {
                    download.setDateTime(boost::posix_time::from_iso_string(historyJson[i].get("DateTime", "").asString()));
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
                it++;
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
        else if(m_length != HistoryLength::Forever)
        {
            boost::gregorian::days daysSinceDownload{ boost::posix_time::second_clock::universal_time().date() - download.getDateTime().date() };
            if(daysSinceDownload > boost::gregorian::days{ static_cast<int>(m_length) })
            {
                return false;
            }
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
        if(it == m_history.end())
        {
            return false;
        }
        else if(m_length != HistoryLength::Forever)
        {
            boost::gregorian::days daysSinceDownload{ boost::posix_time::second_clock::universal_time().date() - download.getDateTime().date() };
            if(daysSinceDownload > boost::gregorian::days{ static_cast<int>(m_length) })
            {
                return false;
            }
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
        m_json.clear();
        save();
        return true;
    }

    void DownloadHistory::updateDisk()
    {
        std::sort(m_history.begin(), m_history.end());
        m_json.clear();
        m_json["Length"] = static_cast<int>(m_length);
        unsigned int i = 0;
        for(const HistoricDownload& download : m_history)
        {
            m_json["History"][i]["URL"] = download.getUrl();
            m_json["History"][i]["Title"] = download.getTitle();
            m_json["History"][i]["Path"] = download.getPath().string();
            m_json["History"][i]["DateTime"] = boost::posix_time::to_iso_string(download.getDateTime());
            i++;
        }
        save();
    }
}