#include <QApplication>
#include <QIcon>
#include <QStyleHints>
#include <Qt>
#include <oclero/qlementine/icons/QlementineIcons.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/style/ThemeManager.hpp>
#include <oclero/qlementine/utils/ImageUtils.hpp>
#include "application.h"
#include "configuration.h"
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
    QIcon::setThemeName("qlementine");
    oclero::qlementine::QlementineStyle* style{ new oclero::qlementine::QlementineStyle{ &application } };
    style->setAutoIconColor(oclero::qlementine::AutoIconColor::TextColor);
    style->setIconPathGetter(oclero::qlementine::icons::fromFreeDesktop);
    style->setToolTipMatchesTheme(true);
    QApplication::setStyle(style);
    oclero::qlementine::ThemeManager* theme_manager{ new oclero::qlementine::ThemeManager{ style } };
    theme_manager->loadDirectory(":/qlementine/resources/themes");
    switch (parabolic::application::State::instance().configuration().theme())
    {
    case parabolic::configuration::Theme::Light:
        theme_manager->setCurrentTheme("Light");
        break;
    case parabolic::configuration::Theme::Dark:
        theme_manager->setCurrentTheme("Legacy Dark");
        break;
    default:
        theme_manager->setCurrentTheme(
                QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ? "Legacy Dark" : "Light");
        break;
    }
    theme_manager->setUseSystemAccentColor(true);
    parabolic::qt::views::MainWindow window;
    window.show();
    return QApplication::exec();
}
