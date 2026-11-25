#include "views/settings_dialog.h"
#include <QDialog>
#include <QKeySequence>
#include <QShortcut>
#include <QVBoxLayout>
#include <QWidget>
#include <oclero/qlementine/icons/Icons16.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/widgets/NavigationView.hpp>
#include "application.h"
#include "translation.h"

namespace parabolic::qt::views
{
    SettingsDialog::SettingsDialog(QWidget* parent)
        : QDialog{ parent },
          m_navigation_view{ new oclero::qlementine::NavigationView(this) }
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        setWindowTitle(translator._("Settings"));
        setModal(true);
        setFixedSize(700, 500);
        m_navigation_view->addPage(new QWidget(m_navigation_view), translator._("General"),
                                   oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                           oclero::qlementine::icons::Icons16::Navigation_Settings)));
        m_navigation_view->addPage(new QWidget(m_navigation_view), translator._("Discovery"),
                                   oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                           oclero::qlementine::icons::Icons16::Navigation_Search)));
        m_navigation_view->addPage(new QWidget(m_navigation_view), translator._("Downloads"),
                                   oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                           oclero::qlementine::icons::Icons16::Action_Download)));
        m_navigation_view->addPage(new QWidget(m_navigation_view), translator._("Extractor"),
                                   oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                           oclero::qlementine::icons::Icons16::File_Plugin)));
        m_navigation_view->addPage(new QWidget(m_navigation_view), translator._("Postprocessing"),
                                   oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                           oclero::qlementine::icons::Icons16::Action_Build)));
        QVBoxLayout* layout{ new QVBoxLayout(this) };
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(m_navigation_view);
        connect(new QShortcut(QKeySequence::Close, this), &QShortcut::activated, this, &QDialog::reject);
    }
}
