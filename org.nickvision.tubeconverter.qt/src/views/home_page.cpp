#include "views/home_page.h"
#include <QApplication>
#include <QObject>
#include <QPalette>
#include <QWidget>
#include "app.h"
#include "helpers/translation_notifier.h"

namespace parabolic::qt::views
{
    HomePage::HomePage(QWidget* parent)
        : QWidget{ parent }
    {
        setAutoFillBackground(true);
        setBackgroundRole(QPalette::Base);
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed,
                this,
                &HomePage::translate);
    }

    void HomePage::translate()
    {
    }
}
