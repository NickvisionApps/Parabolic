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
         * @brief Modes for editing a credential.
         */
        enum class EditMode
        {
            None,
            Add,
            Modify
        };
        /**
         * @brief Handles when the dialog is closed.
         */
        void onClosed();
        /**
         * @brief Handles when the theme preference is changed.
         */
        void onThemeChanged();
        /**
         * @brief Prompts the user to select a cookies file.
         */
        void selectCookiesFile();
        /**
         * @brief Clears the cookies file.
         */
        void clearCookiesFile();
        /**
         * @brief Reloads the postprocessing arguments to show on the page.
         */
        void reloadPostprocessingArguments();
        /**
         * @brief Prompts the user to add a new postprocessing argument.
         */
        void addNewPostprocessingArgument();
        /**
         * @brief Prompts the user to edit a postprocessing argument.
         * @param name The name of the postprocessing argument to edit
         */
        void editPostprocessingArgument(const std::string& name);
        /**
         * @brief Deletes a postprocessing argument.
         * @param name The name of the postprocessing argument to delete
         */
        void deletePostprocessingArgument(const std::string& name);
        /**
         * @brief Confirms an edit to a postprocessing argument.
         */
        void editConfirmPostprocessingArgument();
        std::shared_ptr<Shared::Controllers::PreferencesViewController> m_controller;
        EditMode m_postprocessingArgumentEditMode;
        std::vector<AdwActionRow*> m_postprocessingArgumentRows;
    };
}

#endif //PREFERENCESDIALOG_H
