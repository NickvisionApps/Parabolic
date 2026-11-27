#include "views/history_dialog.h"
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
#include "ui_history_dialog.h"

namespace parabolic::qt::views
{
    HistoryDialog::HistoryDialog(QWidget* parent)
        : QDialog{ parent },
          m_ui{ new Ui::HistoryDialog() }
    {
        m_ui->setupUi(this);
        translate();
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed,
                this,
                &HistoryDialog::translate);
        connect(new QShortcut(QKeySequence::Close, this), &QShortcut::activated, this, &QDialog::reject);
    }

    HistoryDialog::~HistoryDialog()
    {
        delete m_ui;
    }

    void HistoryDialog::translate()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        setWindowTitle(translator._("History"));
    }
}
