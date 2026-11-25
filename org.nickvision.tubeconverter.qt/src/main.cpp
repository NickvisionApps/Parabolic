#include <QAbstractButton>
#include <QApplication>
#include <QCheckBox>
#include <QIcon>
#include <QMessageBox>
#include <QStyleHints>
#include <Qt>
#include <oclero/qlementine/icons/QlementineIcons.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/style/ThemeManager.hpp>
#include <oclero/qlementine/utils/ImageUtils.hpp>
#include "application.h"
#include "configuration.h"
#include "translation.h"
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
    if (parabolic::application::State::instance().configuration().show_disclaimer_on_startup())
    {
        const parabolic::translation::Translator& translator{ parabolic::application::State::instance().translator() };
        QMessageBox disclaimer{ &window };
        QCheckBox* chk_show{ new QCheckBox(translator._("Don't show this message again"), &disclaimer) };
        disclaimer.setIcon(QMessageBox::Information);
        disclaimer.setWindowTitle(translator._("Legal Copyright Disclaimer"));
        disclaimer.setText(translator._("Legal Copyright Disclaimer"));
        disclaimer.setInformativeText(
                translator._("Videos on YouTube and other sites may be subject to DMCA protection. The authors of "
                             "Parabolic do not endorse, and are not responsible for, the use of this application in "
                             "means that will violate these laws."));
        disclaimer.setCheckBox(chk_show);
        disclaimer.setStandardButtons(QMessageBox::Ok);
        disclaimer.button(QMessageBox::Ok)->setText(translator._("I understand"));
        disclaimer.exec();
        parabolic::application::State::instance().configuration().set_show_disclaimer_on_startup(
                !chk_show->isChecked());
    }
    return QApplication::exec();
}
