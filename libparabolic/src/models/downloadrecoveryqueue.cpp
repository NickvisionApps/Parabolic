#include "models/downloadrecoveryqueue.h"

using namespace Nickvision::App;

namespace Nickvision::TubeConverter::Shared::Models
{
    DownloadRecoveryQueue::DownloadRecoveryQueue(const std::string& key, const std::string& appName)
        : DataFileBase{ key, appName }
    {
        
    }
}