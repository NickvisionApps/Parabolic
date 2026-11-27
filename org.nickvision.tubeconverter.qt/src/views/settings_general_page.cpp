#include "views/settings_general_page.h"
#include <QApplication>
#include <QCheckBox>
#include <QCollator>
#include <QComboBox>
#include <QFormLayout>
#include <QFrame>
#include <QList>
#include <QObject>
#include <QPalette>
#include <QScrollArea>
#include <QSignalBlocker>
#include <QString>
#include <QStyleHints>
#include <QVBoxLayout>
#include <QWidget>
#include <Qt>
#include <QtCore/qtypes.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <oclero/qlementine/Common.hpp>
#include <oclero/qlementine/style/ThemeManager.hpp>
#include <oclero/qlementine/widgets/GroupBox.hpp>
#include <oclero/qlementine/widgets/Label.hpp>
#include <string>
#include <unicode/uloc.h>
#include <unicode/umachine.h>
#include <unicode/utypes.h>
#include <utility>
#include <vector>
#include "app.h"
#include "application.h"
#include "configuration.h"
#include "helpers/translation_notifier.h"
#include "history.h"
#include "translation.h"
#include "update.h"

namespace parabolic::qt::views
{
    SettingsGeneralPage::SettingsGeneralPage(QWidget* parent)
        : QScrollArea{ parent },
          m_languages{ application::State::instance().translator().available_languages() },
          m_grp_user_interface{ new oclero::qlementine::GroupBox(this) },
          m_cmb_theme{ new QComboBox(m_grp_user_interface) },
          m_cmb_language{ new QComboBox(m_grp_user_interface) },
          m_grp_updates{ new oclero::qlementine::GroupBox(this) },
          m_chk_automatic_updates{ new QCheckBox(m_grp_updates) },
          m_chk_preview_updates{ new QCheckBox(m_grp_updates) },
          m_grp_history{ new oclero::qlementine::GroupBox(this) },
          m_cmb_history_length{ new QComboBox(m_grp_history) }
    {
        QWidget* content{ new QWidget() };
        QVBoxLayout* layout{ new QVBoxLayout(content) };
        layout->setContentsMargins(24, 20, 24, 20);
        layout->setSpacing(12);
        setFrameShape(QFrame::NoFrame);
        setWidgetResizable(true);
        setWidget(content);
        content->setBackgroundRole(QPalette::Base);
        configuration::Configuration& configuration{ application::State::instance().configuration() };
        QFormLayout* user_interface_layout{ new QFormLayout(m_grp_user_interface) };
        user_interface_layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
        layout->addWidget(m_grp_user_interface);
        m_lbl_theme = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_user_interface);
        user_interface_layout->addRow(m_lbl_theme, m_cmb_theme);
        m_lbl_language = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_user_interface);
        user_interface_layout->addRow(m_lbl_language, m_cmb_language);
        QVBoxLayout* updates_layout{ new QVBoxLayout(m_grp_updates) };
        layout->addWidget(m_grp_updates);
        updates_layout->addWidget(m_chk_automatic_updates);
        updates_layout->addWidget(m_chk_preview_updates);
        m_lbl_preview_updates = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_updates);
        m_lbl_preview_updates->setWordWrap(true);
        m_lbl_preview_updates->setContentsMargins(24, 0, 0, 0);
        updates_layout->addWidget(m_lbl_preview_updates);
        QFormLayout* history_layout{ new QFormLayout(m_grp_history) };
        history_layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
        layout->addWidget(m_grp_history);
        m_lbl_history_length = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_history);
        history_layout->addRow(m_lbl_history_length, m_cmb_history_length);
        layout->addStretch();
        m_cmb_language->addItems(QList<QString>(static_cast<qsizetype>(m_languages.size()) + 1));
        translate();
        std::vector<std::string>::iterator language_it{ std::ranges::find(m_languages,
                                                                          configuration.translation_options().language()) };
        m_cmb_theme->setCurrentIndex(static_cast<int>(configuration.theme()));
        m_cmb_language->setCurrentIndex(
                language_it == m_languages.end() ? 0 : static_cast<int>(language_it - m_languages.begin()) + 1);
        m_chk_automatic_updates->setChecked(configuration.automatically_check_for_updates());
        m_chk_preview_updates->setChecked(configuration.update_options().allow_preview_updates());
        switch (configuration.history_options().length())
        {
        case history::Length::Never:
            m_cmb_history_length->setCurrentIndex(0);
            break;
        case history::Length::OneDay:
            m_cmb_history_length->setCurrentIndex(1);
            break;
        case history::Length::OneWeek:
            m_cmb_history_length->setCurrentIndex(2);
            break;
        case history::Length::OneMonth:
            m_cmb_history_length->setCurrentIndex(3);
            break;
        case history::Length::ThreeMonths:
            m_cmb_history_length->setCurrentIndex(4);
            break;
        case history::Length::SixMonths:
            m_cmb_history_length->setCurrentIndex(5);
            break;
        case history::Length::OneYear:
            m_cmb_history_length->setCurrentIndex(6);
            break;
        case history::Length::Forever:
            m_cmb_history_length->setCurrentIndex(7);
            break;
        }
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed,
                this,
                &SettingsGeneralPage::translate);
        connect(m_cmb_theme, &QComboBox::currentIndexChanged, this, &SettingsGeneralPage::change_theme);
        connect(m_cmb_language, &QComboBox::currentIndexChanged, this, &SettingsGeneralPage::change_language);
        connect(m_chk_automatic_updates, &QCheckBox::toggled, this, &SettingsGeneralPage::save);
        connect(m_chk_preview_updates, &QCheckBox::toggled, this, &SettingsGeneralPage::save);
        connect(m_cmb_history_length, &QComboBox::currentIndexChanged, this, &SettingsGeneralPage::save);
    }

    void SettingsGeneralPage::change_language()
    {
        configuration::Configuration& configuration{ application::State::instance().configuration() };
        translation::Options options{ configuration.translation_options() };
        options.set_language(m_cmb_language->currentIndex() == 0
                                     ? translation::Translator::follow_system_language()
                                     : m_languages[static_cast<std::size_t>(m_cmb_language->currentIndex()) - 1]);
        configuration.set_translation_options(options);
        qobject_cast<App*>(QApplication::instance())->translation_notifier()->translation_changed();
    }

    void SettingsGeneralPage::change_theme()
    {
        oclero::qlementine::ThemeManager* theme_manager{ qobject_cast<App*>(QApplication::instance())->theme_manager() };
        configuration::Theme theme{ static_cast<configuration::Theme>(m_cmb_theme->currentIndex()) };
        application::State::instance().configuration().set_theme(theme);
        switch (theme)
        {
        case configuration::Theme::Light:
            QApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);
            theme_manager->setCurrentTheme("Light");
            break;
        case configuration::Theme::Dark:
            QApplication::styleHints()->setColorScheme(Qt::ColorScheme::Dark);
            theme_manager->setCurrentTheme("Legacy Dark");
            break;
        default:
            QApplication::styleHints()->unsetColorScheme();
            theme_manager->setCurrentTheme(QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ? "Legacy Dark"
                                                                                                              : "Light");
            break;
        }
    }

    void SettingsGeneralPage::save()
    {
        configuration::Configuration& configuration{ application::State::instance().configuration() };
        configuration.set_automatically_check_for_updates(m_chk_automatic_updates->isChecked());
        update::Options update_options{ configuration.update_options() };
        update_options.set_allow_preview_updates(m_chk_preview_updates->isChecked());
        configuration.set_update_options(update_options);
        history::Options history_options{ configuration.history_options() };
        switch (m_cmb_history_length->currentIndex())
        {
        case 0:
            history_options.set_length(history::Length::Never);
            break;
        case 1:
            history_options.set_length(history::Length::OneDay);
            break;
        case 2:
            history_options.set_length(history::Length::OneWeek);
            break;
        case 3:
            history_options.set_length(history::Length::OneMonth);
            break;
        case 4:
            history_options.set_length(history::Length::ThreeMonths);
            break;
        case 5:
            history_options.set_length(history::Length::SixMonths);
            break;
        case 6:
            history_options.set_length(history::Length::OneYear);
            break;
        default:
            history_options.set_length(history::Length::Forever);
            break;
        }
        configuration.set_history_options(history_options);
    }

    void SettingsGeneralPage::translate()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        QSignalBlocker theme_blocker{ m_cmb_theme };
        QSignalBlocker language_blocker{ m_cmb_language };
        QSignalBlocker history_length_blocker{ m_cmb_history_length };
        int theme_index{ m_cmb_theme->currentIndex() };
        int history_length_index{ m_cmb_history_length->currentIndex() };
        std::string selected_language{ m_cmb_language->currentIndex() > 0
                                               ? m_languages[static_cast<std::size_t>(m_cmb_language->currentIndex()) - 1]
                                               : std::string{} };
        m_grp_user_interface->setTitle(translator._("User Interface"));
        m_lbl_theme->setText(QString::fromStdString(translator._("{}:", translator._("Theme"))));
        m_cmb_theme->clear();
        m_cmb_theme->addItems(
                { translator._p("Theme", "Light"), translator._p("Theme", "Dark"), translator._p("Theme", "System") });
        m_cmb_theme->setCurrentIndex(std::max(theme_index, 0));
        m_lbl_language->setText(QString::fromStdString(translator._("{}:", translator._("Translation Language"))));
        m_cmb_language->setItemText(0, translator._p("Language", "System"));
        std::vector<std::pair<QString, std::string>> languages;
        languages.reserve(m_languages.size());
        for (const std::string& language : m_languages)
        {
            std::array<UChar, 128> name{};
            UErrorCode status{ U_ZERO_ERROR };
            int32_t length{ uloc_getDisplayName(language.c_str(),
                                                translator.language().c_str(),
                                                name.data(),
                                                static_cast<int32_t>(name.size()),
                                                &status) };
            languages.emplace_back(U_SUCCESS(status) != 0 && length > 0 ? QString::fromUtf16(name.data(), length)
                                                                        : QString::fromStdString(language),
                                   language);
        }
        QCollator collator;
        std::ranges::sort(languages, collator, &std::pair<QString, std::string>::first);
        for (std::size_t i{ 0 }; i < languages.size(); i++)
        {
            m_languages[i] = languages[i].second;
            m_cmb_language->setItemText(static_cast<int>(i) + 1, languages[i].first);
        }
        std::vector<std::string>::iterator selected_it{ std::ranges::find(m_languages, selected_language) };
        m_cmb_language->setCurrentIndex(selected_it == m_languages.end() || selected_language.empty()
                                                ? 0
                                                : static_cast<int>(selected_it - m_languages.begin()) + 1);
        m_grp_updates->setTitle(translator._("Updates"));
        m_chk_automatic_updates->setText(translator._("Automatically Check for Updates"));
        m_chk_preview_updates->setText(translator._("Allow Preview Updates"));
        m_lbl_preview_updates->setText(translator._("Update the app and dependencies, such as yt-dlp, to beta versions."));
        m_grp_history->setTitle(translator._("History"));
        m_lbl_history_length->setText(QString::fromStdString(translator._("{}:", translator._("Keep Download History For"))));
        m_cmb_history_length->clear();
        m_cmb_history_length->addItems({ translator._("Never"),
                                         translator._("1 Day"),
                                         translator._("1 Week"),
                                         translator._("1 Month"),
                                         translator._("3 Months"),
                                         translator._("6 Months"),
                                         translator._("1 Year"),
                                         translator._("Forever") });
        m_cmb_history_length->setCurrentIndex(std::max(history_length_index, 0));
    }
}
