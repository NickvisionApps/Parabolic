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
         * @brief Handles when the dialog is closed.
         */
        void onClosed();
        std::shared_ptr<Shared::Controllers::AddDownloadDialogController> m_controller;
    };
}

#endif //ADDDOWNLOADDIALOG_H