#include "views/main_window.h"
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QIcon>
#include <QKeySequence>
#include <QMenu>
#include <QMenuBar>
#include <QObject>
#include <QShortcut>
#include <QSizePolicy>
#include <QString>
#include <QToolBar>
#include <QWidget>
#include <Qt>
#include <oclero/qlementine/icons/Icons16.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/widgets/AboutDialog.hpp>
#include <oclero/qlementine/widgets/NavigationSidebar.hpp>
#include <oclero/qlementine/widgets/NavigationView.hpp>
#include <oclero/qlementine/widgets/UnifiedWindow.hpp>
#include "app.h"
#include "application.h"
#include "configuration.h"
#include "helpers/translation_notifier.h"
#include "pages/downloads_page.h"
#include "pages/history_page.h"
#include "pages/home_page.h"
#include "pages/keyring_page.h"
#include "translation.h"
#include "ui_main_window.h"
#include "views/settings_dialog.h"

namespace parabolic::qt::views
{
    MainWindow::MainWindow(QWidget* parent)
        : UnifiedWindow{ parent },
          m_ui{ new Ui::MainWindow() }
    {
        m_ui->setupUi(this);
        setCentralWidget(m_ui->navigation_view);
        setLeadingWidget(m_ui->navigation_view->sidebar());
        setContentCornerRadius(12);
        titleBar()->setContentWidget(m_ui->toolbar);
        m_ui->navigation_view->addPage(new pages::HomePage(m_ui->navigation_view),
                                       {},
                                       oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                               oclero::qlementine::icons::Icons16::Navigation_Home)));
        m_ui->navigation_view->addPage(new pages::DownloadsPage(m_ui->navigation_view),
                                       {},
                                       oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                               oclero::qlementine::icons::Icons16::Action_Download)));
        m_ui->navigation_view->addBottomPage(new pages::HistoryPage(m_ui->navigation_view),
                                             {},
                                             oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                                     oclero::qlementine::icons::Icons16::Misc_Clock)));
        m_ui->navigation_view->addBottomPage(new pages::KeyringPage(m_ui->navigation_view),
                                             {},
                                             oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                                     oclero::qlementine::icons::Icons16::Misc_Password)));
        m_ui->act_toggle_sidebar->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(
                oclero::qlementine::icons::iconPath(oclero::qlementine::icons::Icons16::Navigation_UiPanelLeftCollapse)));
        m_ui->act_add_download->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(
                oclero::qlementine::icons::iconPath(oclero::qlementine::icons::Icons16::Action_Plus)));
        m_ui->act_settings->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(
                oclero::qlementine::icons::iconPath(oclero::qlementine::icons::Icons16::Navigation_Settings)));
        QWidget* spacer{ new QWidget(m_ui->toolbar) };
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        m_ui->toolbar->insertWidget(m_ui->act_settings, spacer);
#ifdef __APPLE__
        QMenu* app_menu{ (new QMenuBar(nullptr))->addMenu(QString()) };
        app_menu->addAction(m_ui->act_about);
        app_menu->addAction(m_ui->act_settings);
#endif
        translate();
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed,
                this,
                &MainWindow::translate);
        connect(new QShortcut(QKeySequence::Close, this), &QShortcut::activated, this, &QWidget::close);
        connect(m_ui->act_about, &QAction::triggered, this, &MainWindow::about);
        connect(m_ui->act_add_download, &QAction::triggered, this, &MainWindow::add_download);
        connect(m_ui->act_settings, &QAction::triggered, this, &MainWindow::settings);
        connect(m_ui->act_toggle_sidebar, &QAction::triggered, this, &MainWindow::toggle_sidebar);
        const configuration::Configuration& configuration{ application::State::instance().configuration() };
        resize(configuration.window_geometry().width(), configuration.window_geometry().height());
        move(configuration.window_geometry().x(), configuration.window_geometry().y());
        if (configuration.window_geometry().is_maximized())
        {
            setWindowState(windowState() | Qt::WindowMaximized);
        }
        if (configuration.collapse_sidebar())
        {
            m_ui->navigation_view->sidebar()->toggleCompact();
        }
    }

    MainWindow::~MainWindow()
    {
        delete m_ui;
    }

    void MainWindow::closeEvent(QCloseEvent* event)
    {
        configuration::Configuration& configuration{ application::State::instance().configuration() };
        if (isMaximized())
        {
            configuration.set_window_geometry({ configuration.window_geometry().width(),
                                                configuration.window_geometry().height(),
                                                true,
                                                configuration.window_geometry().x(),
                                                configuration.window_geometry().y() });
        }
        else
        {
            configuration.set_window_geometry({ width(), height(), false, x(), y() });
        }
        event->accept();
    }

    void MainWindow::about()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        const application::Product& product{ application::State::instance().product() };
        oclero::qlementine::AboutDialog dialog{ this };
        dialog.setWindowTitle(translator._p("AppName", "About Paraboloc"));
        dialog.setIcon(QIcon(":/org.nickvision.tubeconverter.svg"));
        dialog.setApplicationName(QString::fromStdString(product.short_name()));
        dialog.setApplicationVersion(QString::fromStdString(product.version().to_string()));
        dialog.setDescription(QString::fromStdString(product.description()));
        dialog.setWebsiteUrl(QString::fromStdString(product.repo_url()));
        dialog.setLicense("MIT");
        dialog.setCopyright("© 2026 Nickvision");
        dialog.addSocialMediaLink(translator._("Issues"),
                                  QString::fromStdString(product.issues_url()),
                                  oclero::qlementine::appStyle()->makeThemedIcon(
                                          oclero::qlementine::icons::iconPath(oclero::qlementine::icons::Icons16::Misc_Debug)));
        dialog.addSocialMediaLink(translator._("Discussions"),
                                  QString::fromStdString(product.discussions_url()),
                                  oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                          oclero::qlementine::icons::Icons16::Brand_Github)));
        dialog.exec();
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
        bool is_collapsed{ m_ui->navigation_view->sidebar()->compact() };
        application::State::instance().configuration().set_collapse_sidebar(!is_collapsed);
        m_ui->navigation_view->sidebar()->toggleCompact();
        m_ui->act_toggle_sidebar->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                is_collapsed ? oclero::qlementine::icons::Icons16::Navigation_UiPanelLeftExpand
                             : oclero::qlementine::icons::Icons16::Navigation_UiPanelLeftCollapse)));
        m_ui->act_toggle_sidebar->setToolTip(is_collapsed ? translator._("Expand Sidebar") : translator._("Collapse Sidebar"));
    }

    void MainWindow::translate()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        setWindowTitle(translator._p("AppName", "Parabolic"));
        m_ui->navigation_view->sidebar()->setItemText(0, translator._("Home"));
        m_ui->navigation_view->sidebar()->setItemText(1, translator._("Downloads"));
        m_ui->navigation_view->sidebar()->setItemText(2, translator._("History"));
        m_ui->navigation_view->sidebar()->setItemText(3, translator._("Keyring"));
        m_ui->act_toggle_sidebar->setText(translator._("Toggle Sidebar"));
        m_ui->act_toggle_sidebar->setToolTip(m_ui->navigation_view->sidebar()->compact() ? translator._("Expand Sidebar")
                                                                                         : translator._("Collapse Sidebar"));
        m_ui->act_about->setText(translator._("About"));
        m_ui->act_add_download->setText(translator._("Add Download"));
        m_ui->act_add_download->setToolTip(translator._("Add Download"));
        m_ui->act_settings->setText(translator._("Settings"));
        m_ui->act_settings->setToolTip(translator._("Settings"));
    }
}
