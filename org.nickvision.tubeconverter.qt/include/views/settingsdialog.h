#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <memory>
#include <QCloseEvent>
#include <QDialog>
#include <QListWidgetItem>
#include <oclero/qlementine/style/ThemeManager.hpp>
#include "controllers/preferencesviewcontroller.h"

namespace Ui { class SettingsDialog; }

namespace Nickvision::TubeConverter::Qt::Views
{
    /**
     * @brief The settings dialog for the application.
     */
    class SettingsDialog : public QDialog
    {
        Q_OBJECT

    public:
        /**
         * @brief Constructs a SettingsDialog.
         * @param controller The PreferencesViewController
         * @param themeManager The ThemeManager
         * @param parent The parent widget
         */
        SettingsDialog(const std::shared_ptr<Shared::Controllers::PreferencesViewController>& controller, oclero::qlementine::ThemeManager* themeManager, QWidget* parent = nullptr);
        /**
         * @brief Destructs a SettingsDialog.
         */
        ~SettingsDialog();

    protected:
        /**
         * @brief Handles when the dialog is closed.
         * @param event QCloseEvent
         */
        void closeEvent(QCloseEvent* event) override;

    private Q_SLOTS:
        /**
         * @brief Handles when the theme combobox changes.
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
         * @brief Prompts the user to add a postprocessing argument.
         */
        void addPostprocessingArgument();
        /**
         * @brief Handles showing a context menu on the postprocessing arguments list.
         * @param pos QPoint
         */
        void onListPostprocessignArgumentsContextMenu(const QPoint& pos);
        /**
         * @brief Handles when a postprocessing argument in the list was double clicked.
         * @param item The item that was double clicked
         */
        void onPostprocessingArgumentDoubleClicked(QListWidgetItem* item);

    private:
        /**
         * @brief Prompts the user to edit a postprocessing argument.
         * @param name The name of the postprocessing argument to edit
         */
        void editPostprocessingArgument(const QString& name);
        /**
         * @brief Reloads the postprocessing arguments to show on the page.
         */
        void reloadPostprocessingArguments();
        Ui::SettingsDialog* m_ui;
        std::shared_ptr<Shared::Controllers::PreferencesViewController> m_controller;
        oclero::qlementine::ThemeManager* m_themeManager;
    };
}

#endif //SETTINGSDIALOG_H
