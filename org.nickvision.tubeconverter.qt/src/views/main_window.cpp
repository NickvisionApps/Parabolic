#include "views/main_window.h"
#include <QAction>
#include <QIcon>
#include <QSizePolicy>
#include <QToolBar>
#include <QWidget>
#include <Qt>
#include <oclero/qlementine/icons/Icons16.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/widgets/NavigationSidebar.hpp>
#include <oclero/qlementine/widgets/NavigationView.hpp>
#include <oclero/qlementine/widgets/UnifiedWindow.hpp>
#include "application.h"
#include "translation.h"
#include "views/home_page.h"

namespace parabolic::qt::views
{
    MainWindow::MainWindow(QWidget* parent)
        : UnifiedWindow{ parent },
          m_navigation_view{ new oclero::qlementine::NavigationView(this) },
          m_toolbar{ new QToolBar(titleBar()) }
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        m_navigation_view->addPage(new HomePage(m_navigation_view), translator._("Home"),
                                   oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                           oclero::qlementine::icons::Icons16::Navigation_Home)));
        m_toolbar->setMovable(false);
        m_toolbar->setFloatable(false);
        m_toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
        m_toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        m_act_toggle_sidebar =
                m_toolbar->addAction(oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                             oclero::qlementine::icons::Icons16::Navigation_UiPanelLeftCollapse)),
                                     translator._("Collapse Navigation"));
        m_act_toggle_sidebar->setToolTip(translator._("Collapse Navigation"));
        m_act_add_download =
                m_toolbar->addAction(oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                             oclero::qlementine::icons::Icons16::Action_Plus)),
                                     translator._("Add Download"));
        m_act_add_download->setToolTip(translator._("Add Download"));
        titleBar()->setContentWidget(m_toolbar);
        setContentCornerRadius(12);
        resize(1100, 720);
        setCentralWidget(m_navigation_view);
        setLeadingWidget(m_navigation_view->sidebar());
        connect(m_act_toggle_sidebar, &QAction::triggered, this, &MainWindow::toggle_sidebar);
        if (application::State::instance().configuration().collapse_sidebar())
        {
            m_navigation_view->sidebar()->toggleCompact();
        }
    }

    void MainWindow::toggle_sidebar() const
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        bool is_collapsed{ m_navigation_view->sidebar()->compact() };
        m_navigation_view->sidebar()->toggleCompact();
        m_act_toggle_sidebar->setIcon(
                oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                        is_collapsed ? oclero::qlementine::icons::Icons16::Navigation_UiPanelLeftExpand
                                     : oclero::qlementine::icons::Icons16::Navigation_UiPanelLeftCollapse)));
        m_act_toggle_sidebar->setToolTip(is_collapsed ? translator._("Expand Navigation")
                                                      : translator._("Collapse Navigation"));
        application::State::instance().configuration().set_collapse_sidebar(!is_collapsed);
    }
}
