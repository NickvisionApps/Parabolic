#include "views/main_window.h"
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QIcon>
#include <QKeySequence>
#include <QObject>
#include <QShortcut>
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
#include "app.h"
#include "application.h"
#include "configuration.h"
#include "helpers/translation_notifier.h"
#include "translation.h"
#include "views/home_page.h"
#include "views/settings_dialog.h"

namespace parabolic::qt::views
{
    MainWindow::MainWindow(QWidget* parent)
        : UnifiedWindow{ parent },
          m_navigation_view{ new oclero::qlementine::NavigationView(this) },
          m_toolbar{ new QToolBar(titleBar()) }
    {
        setCentralWidget(m_navigation_view);
        setLeadingWidget(m_navigation_view->sidebar());
        setContentCornerRadius(12);
        titleBar()->setContentWidget(m_toolbar);
        m_navigation_view->addPage(new HomePage(m_navigation_view), {},
                                   oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                           oclero::qlementine::icons::Icons16::Navigation_Home)));
        m_navigation_view->addPage(new QWidget(m_navigation_view), {},
                                   oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                           oclero::qlementine::icons::Icons16::Action_Download)));
        m_navigation_view->addBottomPage(new QWidget(m_navigation_view), {},
                                         oclero::qlementine::appStyle()->makeThemedIcon(
                                                 oclero::qlementine::icons::iconPath(
                                                         oclero::qlementine::icons::Icons16::Misc_Clock)));
        m_navigation_view->addBottomPage(new QWidget(m_navigation_view), {},
                                         oclero::qlementine::appStyle()->makeThemedIcon(
                                                 oclero::qlementine::icons::iconPath(
                                                         oclero::qlementine::icons::Icons16::Misc_Password)));
        m_toolbar->setMovable(false);
        m_toolbar->setFloatable(false);
        m_toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
        m_toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        m_act_toggle_sidebar =
                m_toolbar->addAction(oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                             oclero::qlementine::icons::Icons16::Navigation_UiPanelLeftCollapse)),
                                     {});
        m_act_add_download =
                m_toolbar->addAction(oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                             oclero::qlementine::icons::Icons16::Action_Plus)),
                                     {});
        QWidget* spacer{ new QWidget(m_toolbar) };
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        m_toolbar->addWidget(spacer);
        m_act_settings =
                m_toolbar->addAction(oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                             oclero::qlementine::icons::Icons16::Navigation_Settings)),
                                     {});
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed, this, &MainWindow::translate);
        connect(new QShortcut(QKeySequence::Close, this), &QShortcut::activated, this, &QWidget::close);
        connect(m_act_add_download, &QAction::triggered, this, &MainWindow::add_download);
        connect(m_act_settings, &QAction::triggered, this, &MainWindow::settings);
        connect(m_act_toggle_sidebar, &QAction::triggered, this, &MainWindow::toggle_sidebar);
        const configuration::Configuration& configuration{ application::State::instance().configuration() };
        resize(configuration.window_geometry().width(), configuration.window_geometry().height());
        move(configuration.window_geometry().x(), configuration.window_geometry().y());
        if (configuration.window_geometry().is_maximized())
        {
            setWindowState(windowState() | Qt::WindowMaximized);
        }
        if (configuration.collapse_sidebar())
        {
            m_navigation_view->sidebar()->toggleCompact();
        }
    }

    void MainWindow::closeEvent(QCloseEvent* event)
    {
        configuration::Configuration& configuration{ application::State::instance().configuration() };
        if (isMaximized())
        {
            configuration.set_window_geometry(
                    { configuration.window_geometry().width(), configuration.window_geometry().height(), true,
                      configuration.window_geometry().x(), configuration.window_geometry().y() });
        }
        else
        {
            configuration.set_window_geometry({ width(), height(), false, x(), y() });
        }
        event->accept();
    }

    void MainWindow::add_download() const
    {
    }

    void MainWindow::settings()
    {
        SettingsDialog dialog{ this };
        dialog.exec();
    }

    void MainWindow::toggle_sidebar() const
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        bool is_collapsed{ m_navigation_view->sidebar()->compact() };
        application::State::instance().configuration().set_collapse_sidebar(!is_collapsed);
        m_navigation_view->sidebar()->toggleCompact();
        m_act_toggle_sidebar->setIcon(
                oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                        is_collapsed ? oclero::qlementine::icons::Icons16::Navigation_UiPanelLeftExpand
                                     : oclero::qlementine::icons::Icons16::Navigation_UiPanelLeftCollapse)));
        m_act_toggle_sidebar->setToolTip(is_collapsed ? translator._("Expand Sidebar")
                                                      : translator._("Collapse Sidebar"));
    }

    void MainWindow::translate()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        setWindowTitle(translator._p("AppName", "Parabolic"));
        m_navigation_view->sidebar()->setItemText(0, translator._("Home"));
        m_navigation_view->sidebar()->setItemText(1, translator._("Downloads"));
        m_navigation_view->sidebar()->setItemText(2, translator._("History"));
        m_navigation_view->sidebar()->setItemText(3, translator._("Keyring"));
        m_act_toggle_sidebar->setText(translator._("Toggle Sidebar"));
        m_act_toggle_sidebar->setToolTip(m_navigation_view->sidebar()->compact() ? translator._("Expand Sidebar")
                                                                                 : translator._("Collapse Sidebar"));
        m_act_add_download->setText(translator._("Add Download"));
        m_act_add_download->setToolTip(translator._("Add Download"));
        m_act_settings->setText(translator._("Settings"));
        m_act_settings->setToolTip(translator._("Settings"));
    }
}
