#include "views/history_page.h"
#include <QApplication>
#include <QObject>
#include <QPalette>
#include <QWidget>
#include "app.h"
#include "helpers/translation_notifier.h"
#include "ui_history_page.h"

namespace parabolic::qt::views
{
    HistoryPage::HistoryPage(QWidget* parent)
        : QWidget{ parent },
          m_ui{ new Ui::HistoryPage() }
    {
        m_ui->setupUi(this);
        setBackgroundRole(QPalette::Base);
        translate();
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed,
                this,
                &HistoryPage::translate);
    }

    HistoryPage::~HistoryPage()
    {
        delete m_ui;
    }

    void HistoryPage::translate()
    {
    }
}
