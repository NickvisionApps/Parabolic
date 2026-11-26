#include <QApplication>
#include <Qt>
#include "app.h"

int main(int argc, char* argv[])
{
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    parabolic::qt::App app{ argc, argv };
    return app.run();
}
