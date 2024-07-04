#ifndef ADDDOWNLOADDIALOGCONTROLLER_H
#define ADDDOWNLOADDIALOGCONTROLLER_H

#include "models/downloaderoptions.h"

namespace Nickvision::TubeConverter::Shared::Controllers
{
    /**
     * @brief A controller for the AddDownloadDialog.
     */
    class AddDownloadDialogController
    {
    public:
        /**
         * @brief Constructs an AddDownloadDialogController.
         * @param downloaderOptions The DownloaderOptions from the configuration
         */
        AddDownloadDialogController(const Models::DownloaderOptions& downloaderOptions);

    private:
        Models::DownloaderOptions m_downloaderOptions;
    };
}

#endif //ADDDOWNLOADDIALOGCONTROLLER_H