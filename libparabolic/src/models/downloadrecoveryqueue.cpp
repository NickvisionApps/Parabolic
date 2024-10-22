#include "models/downloadrecoveryqueue.h"

using namespace Nickvision::App;

namespace Nickvision::TubeConverter::Shared::Models
{
    DownloadRecoveryQueue::DownloadRecoveryQueue(const std::string& key, const std::string& appName)
        : DataFileBase{ key, appName }
    {
        if(m_json["RecoverableDownloads"].is_array())
        {
            for(const boost::json::value& value : m_json["RecoverableDownloads"].as_array())
            {
                if(!value.is_object())
                {
                    continue;
                }
                boost::json::object download = value.as_object();
                int id{ download["Id"].is_int64() ? static_cast<int>(download["Id"].as_int64()) : -1 };
                if(id == -1)
                {
                    continue;
                }
                m_recoverableDownloads[id] = DownloadOptions(download["Download"].is_object() ? download["Download"].as_object() : boost::json::object());
            }
        }
    }

    const std::unordered_map<int, DownloadOptions>& DownloadRecoveryQueue::getRecoverableDownloads() const
    {
        return m_recoverableDownloads;
    }

    bool DownloadRecoveryQueue::addDownload(int id, const DownloadOptions& downloadOptions)
    {
        if(m_recoverableDownloads.contains(id))
        {
            return false;
        }
        m_recoverableDownloads[id] = downloadOptions;
        updateDisk();
        return true;
    }

    bool DownloadRecoveryQueue::removeDownload(int id)
    {
        if(!m_recoverableDownloads.contains(id))
        {
            return false;
        }
        m_recoverableDownloads.erase(id);
        updateDisk();
        return true;
    }

    bool DownloadRecoveryQueue::clear()
    {
        m_recoverableDownloads.clear();
        m_json.clear();
        save();
        return true;
    }

    void DownloadRecoveryQueue::updateDisk()
    {
        m_json.clear();
        boost::json::array arr;
        for(const std::pair<const int, DownloadOptions>& pair : m_recoverableDownloads)
        {
            boost::json::object obj;
            obj["Id"] = pair.first;
            obj["Download"] = pair.second.toJson();
            arr.push_back(obj);
        }
        m_json["RecoverableDownloads"] = arr;
        save();
    }
}