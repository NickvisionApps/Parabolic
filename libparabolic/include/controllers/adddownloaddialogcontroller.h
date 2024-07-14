#ifndef ADDDOWNLOADDIALOGCONTROLLER_H
#define ADDDOWNLOADDIALOGCONTROLLER_H

#include <libnick/keyring/keyring.h>
#include "models/downloaderoptions.h"
#include "models/previousdownloadoptions.h"

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
         * @param keyring The Keyring to use
         * @param configuration The Configuration to use
         */
        AddDownloadDialogController(const Models::DownloaderOptions& downloaderOptions, Models::PreviousDownloadOptions& previousOptions, std::optional<Keyring::Keyring>& keyring);
        /**
         * @brief Destructs the AddDownloadDialogController.
         */
        ~AddDownloadDialogController();

    private:
        Models::DownloaderOptions m_downloaderOptions;
        Models::PreviousDownloadOptions& m_previousOptions;
        std::optional<Keyring::Keyring>& m_keyring;
    };
}

#endif //ADDDOWNLOADDIALOGCONTROLLER_H