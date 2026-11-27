#include "views/settings_dialog.h"
#include <QApplication>
#include <QDialog>
#include <QKeySequence>
#include <QObject>
#include <QShortcut>
#include <QVBoxLayout>
#include <QWidget>
#include <oclero/qlementine/icons/Icons16.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/widgets/NavigationSidebar.hpp>
#include <oclero/qlementine/widgets/NavigationView.hpp>
#include "app.h"
#include "application.h"
#include "helpers/translation_notifier.h"
#include "translation.h"
#include "views/settings_discovery_page.h"
#include "views/settings_downloads_page.h"
#include "views/settings_extractor_page.h"
#include "views/settings_general_page.h"
#include "views/settings_postprocessing_page.h"

namespace parabolic::qt::views
{
    SettingsDialog::SettingsDialog(QWidget* parent)
        : QDialog{ parent },
          m_navigation_view{ new oclero::qlementine::NavigationView(this) }
    {
        setModal(true);
        setMinimumSize(700, 500);
        QVBoxLayout* layout{ new QVBoxLayout(this) };
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(m_navigation_view);
        m_navigation_view->sidebar()->setExpandedWidth(180);
        m_navigation_view->addPage(new SettingsGeneralPage(m_navigation_view),
                                   {},
                                   oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                           oclero::qlementine::icons::Icons16::Navigation_Settings)));
        m_navigation_view->addPage(new SettingsDiscoveryPage(m_navigation_view),
                                   {},
                                   oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                           oclero::qlementine::icons::Icons16::Navigation_Search)));
        m_navigation_view->addPage(new SettingsDownloadsPage(m_navigation_view),
                                   {},
                                   oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                           oclero::qlementine::icons::Icons16::Action_Download)));
        m_navigation_view->addPage(new SettingsExtractorPage(m_navigation_view),
                                   {},
                                   oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                           oclero::qlementine::icons::Icons16::File_Plugin)));
        m_navigation_view->addPage(new SettingsPostprocessingPage(m_navigation_view),
                                   {},
                                   oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                           oclero::qlementine::icons::Icons16::Action_Build)));
        translate();
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed,
                this,
                &SettingsDialog::translate);
        connect(new QShortcut(QKeySequence::Close, this), &QShortcut::activated, this, &QDialog::reject);
    }

    void SettingsDialog::translate()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        setWindowTitle(translator._("Settings"));
        m_navigation_view->sidebar()->setItemText(0, translator._p("Settings", "General"));
        m_navigation_view->sidebar()->setItemText(1, translator._p("Settings", "Discovery"));
        m_navigation_view->sidebar()->setItemText(2, translator._p("Settings", "Downloads"));
        m_navigation_view->sidebar()->setItemText(3, translator._p("Settings", "Extractor"));
        m_navigation_view->sidebar()->setItemText(4, translator._p("Settings", "Postprocessing"));
    }
}
