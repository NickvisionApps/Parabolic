#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QToolBar>
#include <QWidget>
#include <oclero/qlementine/widgets/NavigationView.hpp>
#include <oclero/qlementine/widgets/UnifiedWindow.hpp>
#include "home_page.h"

namespace parabolic::qt::views
{
    class MainWindow : public oclero::qlementine::UnifiedWindow
    {
    public:
        MainWindow(QWidget* parent = nullptr);
        ~MainWindow() override = default;
        MainWindow(const MainWindow&) = delete;
        MainWindow(MainWindow&&) = delete;
        MainWindow& operator=(const MainWindow&) = delete;
        MainWindow& operator=(MainWindow&&) = delete;

    private:
        void toggle_sidebar() const;
        oclero::qlementine::NavigationView* m_navigation_view{ nullptr };
        QToolBar* m_toolbar{ nullptr };
        QAction* m_act_toggle_sidebar{ nullptr };
        QAction* m_act_add_download{ nullptr };
        HomePage* m_home_page{ nullptr };
    };
}

#endif
