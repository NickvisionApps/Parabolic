#include "pages/home_page.h"
#include <QApplication>
#include <QObject>
#include <QPalette>
#include <QWidget>
#include "app.h"
#include "helpers/translation_notifier.h"
#include "ui_home_page.h"

namespace parabolic::qt::pages
{
    HomePage::HomePage(QWidget* parent)
        : QWidget{ parent },
          m_ui{ new Ui::HomePage() }
    {
        m_ui->setupUi(this);
        setBackgroundRole(QPalette::Base);
        translate();
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed,
                this,
                &HomePage::translate);
    }

    HomePage::~HomePage()
    {
        delete m_ui;
    }

    void HomePage::translate()
    {
    }
}
