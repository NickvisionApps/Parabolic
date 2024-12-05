#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <memory>
#include <QCloseEvent>
#include <QWidget>
#include "controllers/preferencesviewcontroller.h"

namespace Ui { class SettingsPage; }

namespace Nickvision::TubeConverter::QT::Views
{
    /**
     * @brief The settings page for the application.
     */
    class SettingsPage : public QWidget
    {
    Q_OBJECT

    public:
        /**
         * @brief Constructs a SettingsPage.
         * @param controller The PreferencesViewController
         * @param parent The parent widget
         */
        SettingsPage(const std::shared_ptr<Shared::Controllers::PreferencesViewController>& controller, QWidget* parent = nullptr);
        /**
         * @brief Destructs a SettingsPage.
         */
        ~SettingsPage();

    protected:
       /**
         * @brief Handles when the dialog is closed.
         * @param event QCloseEvent
         */
        void closeEvent(QCloseEvent* event) override;

    private Q_SLOT:
        /**
         * @brief Handles when the theme is changed.
         */
        void onThemeChanged(int index);
        /**
         * @brief Prompts the user to select a cookies file.
         */
        void selectCookiesFile();
        /**
         * @brief Clears the cookies file.
         */
        void clearCookiesFile();
        /**
         * @brief Handles when the embed metadata checkbox is toggled.
         * @param checked The new state of the checkbox
         */
        void onEmbedMetadataChanged(bool checked);

    private:
        Ui::SettingsPage* m_ui;
        std::shared_ptr<Shared::Controllers::PreferencesViewController> m_controller;
    };
}

#endif //SETTINGSPAGE_H