#include "views/keyring_dialog.h"
#include <QApplication>
#include <QDialog>
#include <QKeySequence>
#include <QObject>
#include <QShortcut>
#include <QWidget>
#include "app.h"
#include "application.h"
#include "helpers/translation_notifier.h"
#include "translation.h"
#include "ui_keyring_dialog.h"

namespace parabolic::qt::views
{
    KeyringDialog::KeyringDialog(QWidget* parent)
        : QDialog{ parent },
          m_ui{ new Ui::KeyringDialog() }
    {
        m_ui->setupUi(this);
        translate();
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed,
                this,
                &KeyringDialog::translate);
        connect(new QShortcut(QKeySequence::Close, this), &QShortcut::activated, this, &QDialog::reject);
    }

    KeyringDialog::~KeyringDialog()
    {
        delete m_ui;
    }

    void KeyringDialog::translate()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        setWindowTitle(translator._("Keyring"));
    }
}
