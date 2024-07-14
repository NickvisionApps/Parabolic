#include "controllers/adddownloaddialogcontroller.h"

using namespace Nickvision::Keyring;
using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::Shared::Controllers
{
    AddDownloadDialogController::AddDownloadDialogController(const DownloaderOptions& downloaderOptions, PreviousDownloadOptions& previousOptions, std::optional<Keyring::Keyring>& keyring)
        : m_downloaderOptions{ downloaderOptions },
        m_previousOptions{ previousOptions },
        m_keyring{ keyring }
    {
        
    }

    AddDownloadDialogController::~AddDownloadDialogController()
    {
        m_previousOptions.save();   
    }
}