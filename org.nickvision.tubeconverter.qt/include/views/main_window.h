#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QWidget>
#include <oclero/qlementine/widgets/UnifiedWindow.hpp>

namespace Ui
{
    class MainWindow;
}

namespace parabolic::qt::views
{
    class MainWindow : public oclero::qlementine::UnifiedWindow
    {
        Q_OBJECT

    public:
        MainWindow(QWidget* parent = nullptr);
        ~MainWindow() override;
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
        Ui::MainWindow* m_ui{ nullptr };
    };
}

#endif
