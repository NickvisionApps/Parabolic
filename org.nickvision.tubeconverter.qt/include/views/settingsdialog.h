#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <memory>
#include <QCloseEvent>
#include <QDialog>
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

    private:
        Ui::SettingsDialog* m_ui;
        std::shared_ptr<Shared::Controllers::PreferencesViewController> m_controller;
        oclero::qlementine::ThemeManager* m_themeManager;
    };
}

#endif //SETTINGSDIALOG_H
