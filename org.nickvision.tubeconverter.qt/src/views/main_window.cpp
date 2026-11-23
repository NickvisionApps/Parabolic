#include "views/main_window.h"
#include <QWidget>
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

        MainWindow() = default;

        void setupUi(parabolic::qt::views::MainWindow* main_window)
        {
            const parabolic::translation::Translator& translator{
                parabolic::application::State::instance().translator(),
            };
            m_navigation_view = new oclero::qlementine::NavigationView{ main_window };
            m_navigation_view->addPage(new parabolic::qt::views::HomePage{ m_navigation_view }, translator._("Home"),
                                       oclero::qlementine::appStyle()->makeThemedIcon(
                                               oclero::qlementine::icons::iconPath(
                                                       oclero::qlementine::icons::Icons16::Navigation_Home)));
            main_window->resize(800, 600);
            main_window->setCentralWidget(m_navigation_view);
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
        m_ui->setupUi(this);
    }

    MainWindow::~MainWindow()
    {
        delete m_ui;
    }
}
