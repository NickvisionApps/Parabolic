#include "controllers/adddownloaddialogcontroller.h"

using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::Shared::Controllers
{
    AddDownloadDialogController::AddDownloadDialogController(const DownloaderOptions& downloaderOptions)
        : m_downloaderOptions{ downloaderOptions }
    {

    }
}