#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <memory>
#include <adwaita.h>
#include "controllers/preferencesviewcontroller.h"
#include "helpers/dialogbase.h"
#include "models/browsers.h"

namespace Nickvision::TubeConverter::GNOME::Views
{
    /**
     * @brief The preferences dialog for the application.
     */
    class PreferencesDialog : public Helpers::DialogBase
    {
    public:
        /**
         * @brief Constructs a PreferencesDialog.
         * @param controller The PreferencesViewController
         * @param parent The GtkWindow object of the parent window
         */
        PreferencesDialog(const std::shared_ptr<Shared::Controllers::PreferencesViewController>& controller, GtkWindow* parent);

    private:
        /**
         * @brief Handles when the dialog is closed.
         */
        void onClosed();
        /**
         * @brief Handles when the theme preference is changed.
         */
        void onThemeChanged();
        /**
         * @brief Prompts the user to open a cookies file.
         */
        void selectCookiesFile();
        /**
         * @brief Clears the cookies file.
         */
        void clearCookiesFile();
        /**
         * @brief Opens the link to the cookies extension for the browser.
         * @param browser The browser to open the link for
         */
        void cookiesExtension(Shared::Models::Browsers browser);
        std::shared_ptr<Shared::Controllers::PreferencesViewController> m_controller;
    };
}

#endif //PREFERENCESDIALOG_H
