#include "views/main_window.h"
#include <QAction>
#include <QIcon>
#include <QObject>
#include <QSizePolicy>
#include <QString>
#include <QToolBar>
#include <QWidget>
#include <Qt>
#include <oclero/qlementine/icons/Icons16.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/style/ThemeManager.hpp>
#include <oclero/qlementine/widgets/NavigationSidebar.hpp>
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
        QAction* m_toggle_navigation{ nullptr };
        QAction* m_add_download{ nullptr };
        QWidget* m_toolbar_spacer{ nullptr };
        QAction* m_toggle_theme{ nullptr };

        MainWindow(oclero::qlementine::ThemeManager* theme_manager)
            : m_theme_manager{ theme_manager }
        {
        }

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
            m_toggle_navigation = m_toolbar->addAction(oclero::qlementine::appStyle()->makeThemedIcon(
                                                               oclero::qlementine::icons::iconPath(
                                                                       oclero::qlementine::icons::Icons16::
                                                                               Navigation_UiPanelLeftCollapse)),
                                                       translator._("Collapse Navigation"));
            m_toggle_navigation->setToolTip(translator._("Collapse Navigation"));
            m_add_download = m_toolbar->addAction(oclero::qlementine::appStyle()->makeThemedIcon(
                                                          oclero::qlementine::icons::iconPath(
                                                                  oclero::qlementine::icons::Icons16::Action_Plus)),
                                                  translator._("Add Download"));
            m_add_download->setToolTip(translator._("Add Download"));
            m_toolbar_spacer = new QWidget{ m_toolbar };
            m_toolbar_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            m_toolbar->addWidget(m_toolbar_spacer);
            m_toggle_theme = m_toolbar->addAction(QIcon{}, QString{});
            update_toggle_theme();
            QObject::connect(m_toggle_theme, &QAction::triggered, m_toggle_theme, [this]()
            {
                m_theme_manager->setCurrentTheme(m_theme_manager->currentTheme() == "Light" ? "Legacy Dark" : "Light");
            });
            QObject::connect(m_theme_manager, &oclero::qlementine::ThemeManager::currentThemeChanged, m_toggle_theme,
                             [this]()
            {
                update_toggle_theme();
            });
            main_window->titleBar()->setContentWidget(m_toolbar);
            main_window->setContentCornerRadius(12);
            main_window->resize(1100, 720);
            main_window->setCentralWidget(m_navigation_view);
            m_navigation_view->sidebar()->setBorderVisible(false);
            QObject::connect(m_toggle_navigation, &QAction::triggered, m_navigation_view->sidebar(),
                             &oclero::qlementine::NavigationSidebar::toggleCompact);
            QObject::connect(m_navigation_view->sidebar(), &oclero::qlementine::NavigationSidebar::compactChanged,
                             m_toggle_navigation, [this, &translator]()
            {
                m_toggle_navigation->setIcon(
                        oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                m_navigation_view->sidebar()->compact()
                                        ? oclero::qlementine::icons::Icons16::Navigation_UiPanelLeftExpand
                                        : oclero::qlementine::icons::Icons16::Navigation_UiPanelLeftCollapse)));
                m_toggle_navigation->setToolTip(m_navigation_view->sidebar()->compact()
                                                        ? translator._("Expand Navigation")
                                                        : translator._("Collapse Navigation"));
            });
            main_window->setLeadingWidget(m_navigation_view->sidebar());
        }

    private:
        oclero::qlementine::ThemeManager* m_theme_manager{ nullptr };

        void update_toggle_theme()
        {
            const parabolic::translation::Translator& translator{
                parabolic::application::State::instance().translator(),
            };
            m_toggle_theme->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                    m_theme_manager->currentTheme() != "Light" ? oclero::qlementine::icons::Icons16::Misc_Sun
                                                               : oclero::qlementine::icons::Icons16::Misc_Moon)));
            m_toggle_theme->setText(m_theme_manager->currentTheme() != "Light" ? translator._("Switch to Light Theme")
                                                                               : translator._("Switch to Dark Theme"));
            m_toggle_theme->setToolTip(m_toggle_theme->text());
        }
    };
}

namespace parabolic::qt::views
{
    MainWindow::MainWindow(oclero::qlementine::ThemeManager* theme_manager, QWidget* parent)
        : UnifiedWindow{ parent },
          m_ui{ new ui::MainWindow(theme_manager) }
    {
        m_ui->setup_ui(this);
    }

    MainWindow::~MainWindow()
    {
        delete m_ui;
    }
}
