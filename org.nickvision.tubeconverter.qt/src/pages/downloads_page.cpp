#include "pages/downloads_page.h"
#include <QApplication>
#include <QObject>
#include <QPalette>
#include <QWidget>
#include "app.h"
#include "helpers/translation_notifier.h"
#include "ui_downloads_page.h"

namespace parabolic::qt::pages
{
    DownloadsPage::DownloadsPage(QWidget* parent)
        : QWidget{ parent },
          m_ui{ new Ui::DownloadsPage() }
    {
        m_ui->setupUi(this);
        setBackgroundRole(QPalette::Base);
        translate();
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed,
                this,
                &DownloadsPage::translate);
    }

    DownloadsPage::~DownloadsPage()
    {
        delete m_ui;
    }

    void DownloadsPage::translate()
    {
    }
}
