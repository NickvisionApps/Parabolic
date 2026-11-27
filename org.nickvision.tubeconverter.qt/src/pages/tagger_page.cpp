#include "pages/tagger_page.h"
#include <QApplication>
#include <QObject>
#include <QPalette>
#include <QWidget>
#include "app.h"
#include "helpers/translation_notifier.h"
#include "ui_tagger_page.h"

namespace parabolic::qt::pages
{
    TaggerPage::TaggerPage(QWidget* parent)
        : QWidget{ parent },
          m_ui{ new Ui::TaggerPage() }
    {
        m_ui->setupUi(this);
        setBackgroundRole(QPalette::Base);
        translate();
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed,
                this,
                &TaggerPage::translate);
    }

    TaggerPage::~TaggerPage()
    {
        delete m_ui;
    }

    void TaggerPage::translate()
    {
    }
}
