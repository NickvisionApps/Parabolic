#include "models/downloadrecoveryqueue.h"

using namespace Nickvision::App;

namespace Nickvision::TubeConverter::Shared::Models
{
    DownloadRecoveryQueue::DownloadRecoveryQueue(const std::string& key, const std::string& appName)
        : DataFileBase{ key, appName }
    {
        if(m_json["RecoverableDownloads"].is_array())
        {
            m_recoverableDownloads.reserve(m_json["RecoverableDownloads"].as_array().size());
            m_needsCredentials.reserve(m_json["RecoverableDownloads"].as_array().size());
            for(const boost::json::value& value : m_json["RecoverableDownloads"].as_array())
            {
                if(!value.is_object())
                {
                    continue;
                }
                boost::json::object recoverableDownload = value.as_object();
                int id{ recoverableDownload["Id"].is_int64() ? static_cast<int>(recoverableDownload["Id"].as_int64()) : -1 };
                if(id == -1)
                {
                    continue;
                }
                m_recoverableDownloads[id] = DownloadOptions(recoverableDownload["Download"].is_object() ? recoverableDownload["Download"].as_object() : boost::json::object());
                m_needsCredentials[id] = recoverableDownload["NeedsCredentials"].is_bool() ? recoverableDownload["NeedsCredentials"].as_bool() : false;
            }
        }
    }

    const std::unordered_map<int, DownloadOptions>& DownloadRecoveryQueue::getRecoverableDownloads() const
    {
        return m_recoverableDownloads;
    }

    bool DownloadRecoveryQueue::needsCredential(int id) const
    {
        if(!m_needsCredentials.contains(id))
        {
            return false;
        }
        return m_needsCredentials.at(id);
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
            obj["Download"] = pair.second.toJson(false);
            obj["NeedsCredentials"] = pair.second.getCredential().has_value();
            arr.push_back(obj);
        }
        m_json["RecoverableDownloads"] = arr;
        save();
    }
}