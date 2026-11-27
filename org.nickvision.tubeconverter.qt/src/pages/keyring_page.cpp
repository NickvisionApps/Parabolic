#include "pages/keyring_page.h"
#include <QApplication>
#include <QObject>
#include <QPalette>
#include <QWidget>
#include "app.h"
#include "helpers/translation_notifier.h"
#include "ui_keyring_page.h"

namespace parabolic::qt::pages
{
    KeyringPage::KeyringPage(QWidget* parent)
        : QWidget{ parent },
          m_ui{ new Ui::KeyringPage() }
    {
        m_ui->setupUi(this);
        setBackgroundRole(QPalette::Base);
        translate();
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed,
                this,
                &KeyringPage::translate);
    }

    KeyringPage::~KeyringPage()
    {
        delete m_ui;
    }

    void KeyringPage::translate()
    {
    }
}
