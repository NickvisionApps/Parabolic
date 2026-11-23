#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <oclero/qlementine/widgets/UnifiedWindow.hpp>

namespace ui
{
    class MainWindow;
}

namespace parabolic::qt::views
{
    class MainWindow : public oclero::qlementine::UnifiedWindow
    {
    public:
        MainWindow(QWidget* parent = nullptr);
        ~MainWindow() override;
        MainWindow(const MainWindow&) = delete;
        MainWindow(MainWindow&&) = delete;
        MainWindow& operator=(const MainWindow&) = delete;
        MainWindow& operator=(MainWindow&&) = delete;

    private:
        ui::MainWindow* m_ui{ nullptr };
    };
}

#endif
