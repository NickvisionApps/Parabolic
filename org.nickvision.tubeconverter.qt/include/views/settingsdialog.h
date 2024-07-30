#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <memory>
#include <QCloseEvent>
#include <QDialog>
#include "controllers/preferencesviewcontroller.h"

namespace Ui { class SettingsDialog; }

namespace Nickvision::TubeConverter::QT::Views
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
         * @param parent The parent widget
         */
        SettingsDialog(const std::shared_ptr<Shared::Controllers::PreferencesViewController>& controller, QWidget* parent = nullptr);
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
         * @brief Handles when the page is changed.
         * @param index The index of the new page
         */
        void onPageChanged(int index);
        /**
         * @brief Prompts the user to select a cookies file.
         */
        void selectCookiesFile();
        /**
         * @brief Clears the selected cookies file.
         */
        void clearCookiesFile();
        /**
         * @brief Shows information about the cookies files.
         */
        void cookiesFileInformation();

    private:
        Ui::SettingsDialog* m_ui;
        std::shared_ptr<Shared::Controllers::PreferencesViewController> m_controller;
    };
}

#endif //SETTINGSDIALOG_H