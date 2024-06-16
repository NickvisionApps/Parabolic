#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <memory>
#include <adwaita.h>
#include "controllers/preferencesviewcontroller.h"
#include "helpers/dialogbase.h"

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
         * @brief Handles when the theme preference is changed.
         */
        void onThemeChanged();
        std::shared_ptr<Shared::Controllers::PreferencesViewController> m_controller;
    };
}

#endif //PREFERENCESDIALOG_H
