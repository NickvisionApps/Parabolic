#include "views/main_window.h"
#include <QAction>
#include <QSizePolicy>
#include <QToolBar>
#include <QWidget>
#include <Qt>
#include <oclero/qlementine/icons/Icons16.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/widgets/NavigationView.hpp>
#include <oclero/qlementine/widgets/UnifiedWindow.hpp>
#include "application.h"
#include "translation.h"
#include "views/home_page.h"

namespace ui
{
    class MainWindow
    {
    public:
        oclero::qlementine::NavigationView* m_navigation_view{ nullptr };
        QToolBar* m_toolbar{ nullptr };
        QAction* m_add_download{ nullptr };

        MainWindow() = default;

        void setup_ui(parabolic::qt::views::MainWindow* main_window)
        {
            const parabolic::translation::Translator& translator{
                parabolic::application::State::instance().translator(),
            };
            m_navigation_view = new oclero::qlementine::NavigationView{ main_window };
            m_navigation_view->addPage(new parabolic::qt::views::HomePage(m_navigation_view), translator._("Home"),
                                       oclero::qlementine::appStyle()->makeThemedIcon(
                                               oclero::qlementine::icons::iconPath(
                                                       oclero::qlementine::icons::Icons16::Navigation_Home)));
            m_toolbar = new QToolBar{ main_window->titleBar() };
            m_toolbar->setMovable(false);
            m_toolbar->setFloatable(false);
            m_toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
            m_toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            m_add_download = m_toolbar->addAction(oclero::qlementine::appStyle()->makeThemedIcon(
                                                          oclero::qlementine::icons::iconPath(
                                                                  oclero::qlementine::icons::Icons16::Action_Plus)),
                                                  translator._("Add Download"));
            m_add_download->setToolTip(translator._("Add Download"));
            main_window->titleBar()->setContentWidget(m_toolbar);
            main_window->resize(1100, 720);
            main_window->setCentralWidget(m_navigation_view);
            m_navigation_view->sidebar()->setBorderVisible(false);
            main_window->setLeadingWidget(m_navigation_view->sidebar());
        }
    };
}

namespace parabolic::qt::views
{
    MainWindow::MainWindow(QWidget* parent)
        : UnifiedWindow{ parent },
          m_ui{ new ui::MainWindow() }
    {
        m_ui->setup_ui(this);
    }

    MainWindow::~MainWindow()
    {
        delete m_ui;
    }
}
