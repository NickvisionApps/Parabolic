#include "pages/updates_page.h"
#include <QApplication>
#include <QObject>
#include <QPalette>
#include <QWidget>
#include "app.h"
#include "helpers/translation_notifier.h"
#include "ui_updates_page.h"

namespace parabolic::qt::pages
{
    UpdatesPage::UpdatesPage(QWidget* parent)
        : QWidget{ parent },
          m_ui{ new Ui::UpdatesPage() }
    {
        m_ui->setupUi(this);
        setBackgroundRole(QPalette::Base);
        translate();
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed,
                this,
                &UpdatesPage::translate);
    }

    UpdatesPage::~UpdatesPage()
    {
        delete m_ui;
    }

    void UpdatesPage::translate()
    {
    }
}
