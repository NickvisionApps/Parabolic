#include "models/downloadhistory.h"
#include <algorithm>

using namespace Nickvision::App;

namespace Nickvision::TubeConverter::Shared::Models
{
    DownloadHistory::DownloadHistory(const std::string& key)
        : ConfigurationBase{ key }
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
            m_history.push_back(download);
        }
        std::sort(m_history.begin(), m_history.end());
    }

    const std::vector<HistoricDownload>& DownloadHistory::getHistory() const
    {
        return m_history;
    }

    bool DownloadHistory::addDownload(const HistoricDownload& download)
    {
        if(std::find(m_history.begin(), m_history.end(), download) != m_history.end())
        {
            return false;
        }
        m_history.push_back(download);
        updateDisk();
        return true;
    }

    bool DownloadHistory::updateDownload(const HistoricDownload& download)
    {
        std::vector<HistoricDownload>::iterator it{ std::find(m_history.begin(), m_history.end(), download) };
        if(it == m_history.end())
        {
            return false;
        }
        *it = download;
        updateDisk();
        return true;
    }

    bool DownloadHistory::removeDownload(const HistoricDownload& download)
    {
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