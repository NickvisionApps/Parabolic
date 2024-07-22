#ifndef ADDDOWNLOADDIALOG_H
#define ADDDOWNLOADDIALOG_H

#include <memory>
#include <adwaita.h>
#include "controllers/adddownloaddialogcontroller.h"
#include "helpers/dialogbase.h"

namespace Nickvision::TubeConverter::GNOME::Views
{
    /**
     * @brief The add download dialog for the application.
     */
    class AddDownloadDialog : public Helpers::DialogBase
    {
    public:
        /**
         * @brief Constructs a AddDownloadDialog.
         * @param controller The AddDownloadDialogController
         * @param parent The GtkWindow object of the parent window
         */
        AddDownloadDialog(const std::shared_ptr<Shared::Controllers::AddDownloadDialogController>& controller, GtkWindow* parent);

    private:
        /**
         * @brief Handles when the url text is changed.
         */
        void onTxtUrlChanged();
        /**
         * @brief Handles when the credential combobox is changed.
         */
        void onCmbCredentialChanged();
        /**
         * @brief Validates the url.
         */
        void validateUrl();
        /**
         * @brief Handles when a url is validated.
         */
        void onUrlValidated();
        /**
         * @brief Handles when the single file type combobox is changed.
         */
        void onFileTypeSingleChanged();
        /**
         * @brief Goes back to the single download page.
         */
        void backSingle();
        /**
         * @brief Shows the advanced options for a single download.
         */
        void advancedOptionsSingle();
        /**
         * @brief Prompts the user to select a folder to save a single download.
         */
        void selectSaveFolderSingle();
        /**
         * @brief Reverts the filename for a single download to its original title.
         */
        void revertFilenameSingle();
        /**
         * @brief Reverts the start time for a single download to its original value.
         */
        void revertStartTimeSingle();
        /**
         * @brief Reverts the end time for a single download to its original value.
         */
        void revertEndTimeSingle();
        std::shared_ptr<Shared::Controllers::AddDownloadDialogController> m_controller;
    };
}

#endif //ADDDOWNLOADDIALOG_H