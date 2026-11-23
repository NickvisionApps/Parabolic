#include <QApplication>
#include <QObject>
#include <QStyleHints>
#include <Qt>
#include <oclero/qlementine/icons/QlementineIcons.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/style/Theme.hpp>
#include <oclero/qlementine/style/ThemeManager.hpp>
#include "application.h"
#include "views/main_window.h"

int main(int argc, char* argv[])
{
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QApplication application{ argc, argv };
    QApplication::setApplicationName("Parabolic");
    QApplication::setApplicationDisplayName(
            parabolic::application::State::instance().translator()._p("AppName", "Parabolic"));
    QApplication::setOrganizationName("Nickvision");
    QApplication::setOrganizationDomain("nickvision.org");
    QApplication::setDesktopFileName("org.nickvision.tubeconverter");
    oclero::qlementine::icons::initializeIconTheme();
    oclero::qlementine::QlementineStyle* style{ new oclero::qlementine::QlementineStyle{ &application } };
    oclero::qlementine::ThemeManager* theme_manager{ new oclero::qlementine::ThemeManager{ style } };
    QApplication::setStyle(style);
    QObject::connect(style, &oclero::qlementine::QlementineStyle::themeChanged, style, [style]()
    {
        if (style->theme().titleBarBorderColor.alpha() != 0)
        {
            oclero::qlementine::Theme theme{ style->theme() };
            theme.titleBarBorderColor = Qt::transparent;
            style->setTheme(theme);
        }
    });
    theme_manager->loadDirectory(":/qlementine/resources/themes");
    theme_manager->setCurrentTheme(QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ? "Legacy Dark"
                                                                                                      : "Light");
    parabolic::qt::views::MainWindow window;
    window.show();
    return QApplication::exec();
}
