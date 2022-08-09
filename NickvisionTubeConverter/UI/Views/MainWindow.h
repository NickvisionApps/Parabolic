#pragma once

#include <QMainWindow>
#include <QShowEvent>
#include "ui_MainWindow.h"
#include "BrowsePage.h"
#include "HomePage.h"
#include "Pages.h"
#include "../../Models/DependencyManager.h"
#include "../../Models/Theme.h"
#include "../../Update/Updater.h"

namespace NickvisionTubeConverter::UI::Views
{
    /// <summary>
    /// The MainWindow
    /// </summary>
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        /// <summary>
        /// Constructs a MainWindow
        /// </summary>
        /// <param name="parent">The parent of the widget, if any</param>
        MainWindow(QWidget* parent = nullptr);

    protected:
        /// <summary>
        /// Occurs when the window is shown
        /// </summary>
        /// <param name="event">QShowEvent</param>
        void showEvent(QShowEvent* event) override;

    private slots:
        /// <summary>
        /// Navigate to home page
        /// </summary>
        void on_navHome_clicked();
        /// <summary>
        /// Navigate to browse page
        /// </summary>
        void on_navBrowse_clicked();
        /// <summary>
        /// Navigate to downloads page
        /// </summary>
        void on_navDownloads_clicked();
        /// <summary>
        /// Navigate to logs page
        /// </summary>
        void on_navLogs_clicked();
        /// <summary>
        /// Checks for an application update
        /// </summary>
        void on_navCheckForUpdates_clicked();
        /// <summary>
        /// Opens the SettingsDialog
        /// </summary>
        void on_navSettings_clicked();
        /// <summary>
        /// Opens an AboutDialog
        /// </summary>
        void on_navAbout_clicked();

    private:
        //==Vars==//
        bool m_opened;
        NickvisionTubeConverter::Models::Theme m_currentTheme;
        NickvisionTubeConverter::Update::Updater m_updater;
        NickvisionTubeConverter::Models::DependencyManager m_dependencyManager;
        //==UI==//
        Ui::MainWindow m_ui;
        HomePage m_homePage;
        BrowsePage m_browsePage;
        //==Functions==//
        /// <summary>
        /// Refreshes the theme of the window
        /// </summary>
        void refreshTheme();
        /// <summary>
        /// Changes the page on the window
        /// </summary>
        /// <param name="page">The page to change to</param>
        void changePage(Pages page);
    };
}
