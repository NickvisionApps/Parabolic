#include "models/downloadrecoveryqueue.h"

using namespace Nickvision::Helpers;

namespace Nickvision::TubeConverter::Shared::Models
{
    DownloadRecoveryQueue::DownloadRecoveryQueue(const std::filesystem::path& path)
        : JsonFileBase{ path }
    {
        boost::json::array jsonDownloads = get<boost::json::array>("RecoverableDownloads", {});
        if(jsonDownloads.size() > 0)
        {
            m_queue.reserve(jsonDownloads.size());
            for(const boost::json::value& value : jsonDownloads)
            {
                if(!value.is_object())
                {
                    continue;
                }
                boost::json::object recoverableDownload = value.as_object();
                int id{ recoverableDownload["Id"].is_int64() ? static_cast<int>(recoverableDownload["Id"].as_int64()) : false };
                DownloadOptions options{ recoverableDownload["Download"].is_object() ? DownloadOptions(recoverableDownload["Download"].as_object()) : DownloadOptions() };
                bool needsCredentials{ recoverableDownload["NeedsCredential"].is_bool() ? recoverableDownload["NeedsCredential"].as_bool() : false };
                m_queue.emplace(id, std::make_pair(options, needsCredentials));
            }
        }
    }

    std::unordered_map<int, std::pair<DownloadOptions, bool>>& DownloadRecoveryQueue::getRecoverableDownloads()
    {
        return m_queue;
    }

    bool DownloadRecoveryQueue::add(int id, const DownloadOptions& downloadOptions)
    {
        m_queue.emplace(id, std::make_pair(downloadOptions, downloadOptions.getCredential().has_value()));
        updateDisk();
        return true;
    }

    bool DownloadRecoveryQueue::remove(int id)
    {
        if(!m_queue.contains(id))
        {
            return false;
        }
        m_queue.erase(id);
        updateDisk();
        return true;
    }

    bool DownloadRecoveryQueue::clear()
    {
        m_queue.clear();
        updateDisk();
        return true;
    }

    void DownloadRecoveryQueue::updateDisk()
    {
        boost::json::array arr;
        for(const std::pair<const int, std::pair<DownloadOptions, bool>>& pair : m_queue)
        {
            boost::json::object obj;
            obj["Id"] = pair.first;
            obj["Download"] = pair.second.first.toJson(false);
            obj["NeedsCredential"] = pair.second.second;
            arr.push_back(obj);
        }
        set("RecoverableDownloads", arr);
        save();
    }
}
