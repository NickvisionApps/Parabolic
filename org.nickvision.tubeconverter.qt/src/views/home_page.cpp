#include "views/home_page.h"
#include <QPalette>
#include <QWidget>

namespace parabolic::qt::views
{
    HomePage::HomePage(QWidget* parent)
        : QWidget{ parent }
    {
        setAutoFillBackground(true);
        setBackgroundRole(QPalette::Base);
    }
}
