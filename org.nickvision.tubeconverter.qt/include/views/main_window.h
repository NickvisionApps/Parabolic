#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QCloseEvent>
#include <QToolBar>
#include <QWidget>
#include <oclero/qlementine/widgets/NavigationView.hpp>
#include <oclero/qlementine/widgets/UnifiedWindow.hpp>

namespace parabolic::qt::views
{
    class MainWindow : public oclero::qlementine::UnifiedWindow
    {
        Q_OBJECT

    public:
        MainWindow(QWidget* parent = nullptr);
        ~MainWindow() override = default;
        MainWindow(const MainWindow&) = delete;
        MainWindow(MainWindow&&) = delete;
        MainWindow& operator=(const MainWindow&) = delete;
        MainWindow& operator=(MainWindow&&) = delete;

    protected:
        void closeEvent(QCloseEvent* event) override;

    private:
        Q_SLOT void about();
        Q_SLOT void add_download() const;
        Q_SLOT void settings();
        Q_SLOT void toggle_sidebar() const;
        Q_SLOT void translate();
        oclero::qlementine::NavigationView* m_navigation_view{ nullptr };
        QToolBar* m_toolbar{ nullptr };
        QAction* m_act_about{ nullptr };
        QAction* m_act_add_download{ nullptr };
        QAction* m_act_settings{ nullptr };
        QAction* m_act_toggle_sidebar{ nullptr };
    };
}

#endif
