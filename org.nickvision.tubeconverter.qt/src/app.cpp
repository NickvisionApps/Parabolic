#include "app.h"
#include <QAbstractButton>
#include <QApplication>
#include <QCheckBox>
#include <QIcon>
#include <QMessageBox>
#include <QObject>
#include <QStyleHints>
#include <QWidget>
#include <Qt>
#include <oclero/qlementine/icons/QlementineIcons.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/style/ThemeManager.hpp>
#include <oclero/qlementine/utils/ImageUtils.hpp>
#include "application.h"
#include "configuration.h"
#include "helpers/translation_notifier.h"
#include "translation.h"
#include "views/main_window.h"

namespace parabolic::qt
{
    App::App(int& argc, char** argv)
        : QApplication{ argc, argv }
    {
        setApplicationName("Parabolic");
        setApplicationDisplayName(application::State::instance().translator()._p("AppName", "Parabolic"));
        setOrganizationName("Nickvision");
        setOrganizationDomain("nickvision.org");
        setDesktopFileName("org.nickvision.tubeconverter");
#ifndef __APPLE__
        setWindowIcon(QIcon(":/org.nickvision.tubeconverter.svg"));
#endif
        oclero::qlementine::icons::initializeIconTheme();
        QIcon::setThemeName("qlementine");
        oclero::qlementine::QlementineStyle* style{ new oclero::qlementine::QlementineStyle{ this } };
        style->setAutoIconColor(oclero::qlementine::AutoIconColor::TextColor);
        style->setIconPathGetter(oclero::qlementine::icons::fromFreeDesktop);
        style->setToolTipMatchesTheme(true);
        setStyle(style);
        m_theme_manager = new oclero::qlementine::ThemeManager{ style, this };
        m_theme_manager->loadDirectory(":/qlementine/resources/themes");
        connect(m_theme_manager,
                &oclero::qlementine::ThemeManager::systemColorSchemeChanged,
                this,
                &App::system_color_scheme_changed);
        switch (application::State::instance().configuration().theme())
        {
        case configuration::Theme::Light:
            styleHints()->setColorScheme(Qt::ColorScheme::Light);
            m_theme_manager->setCurrentTheme("Light");
            break;
        case configuration::Theme::Dark:
            styleHints()->setColorScheme(Qt::ColorScheme::Dark);
            m_theme_manager->setCurrentTheme("Legacy Dark");
            break;
        default:
            styleHints()->unsetColorScheme();
            m_theme_manager->setCurrentTheme(styleHints()->colorScheme() == Qt::ColorScheme::Dark ? "Legacy Dark" : "Light");
            break;
        }
        m_theme_manager->setUseSystemAccentColor(true);
    }

    oclero::qlementine::ThemeManager* App::theme_manager() const
    {
        return m_theme_manager;
    }

    helpers::TranslationNotifier* App::translation_notifier()
    {
        return &m_translation_notifier;
    }

    int App::run()
    {
        views::MainWindow window;
        window.show();
        configuration::Configuration& configuration{ application::State::instance().configuration() };
        if (configuration.show_disclaimer_on_startup())
        {
            const translation::Translator& translator{ application::State::instance().translator() };
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
            configuration.set_show_disclaimer_on_startup(!chk_show->isChecked());
        }
        return exec();
    }

    void App::system_color_scheme_changed(Qt::ColorScheme color_scheme)
    {
        if (application::State::instance().configuration().theme() == configuration::Theme::System)
        {
            m_theme_manager->setCurrentTheme(color_scheme == Qt::ColorScheme::Dark ? "Legacy Dark" : "Light");
        }
    }
}
