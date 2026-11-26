#include "views/settings_dialog.h"
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDesktopServices>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QLineEdit>
#include <QList>
#include <QListWidget>
#include <QMessageBox>
#include <QObject>
#include <QPalette>
#include <QPushButton>
#include <QScrollArea>
#include <QShortcut>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QString>
#include <QStyleHints>
#include <QToolButton>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>
#include <Qt>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <oclero/qlementine/Common.hpp>
#include <oclero/qlementine/icons/Icons16.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/style/ThemeManager.hpp>
#include <oclero/qlementine/widgets/Label.hpp>
#include <oclero/qlementine/widgets/NavigationSidebar.hpp>
#include <oclero/qlementine/widgets/NavigationView.hpp>
#include <optional>
#include <string>
#include <thread>
#include <unicode/uloc.h>
#include <unicode/umachine.h>
#include <unicode/utypes.h>
#include <utility>
#include <vector>
#include "app.h"
#include "application.h"
#include "aria2.h"
#include "audio.h"
#include "configuration.h"
#include "cookies.h"
#include "discovery.h"
#include "download.h"
#include "extractor.h"
#include "helpers/translation_notifier.h"
#include "history.h"
#include "postprocessing.h"
#include "subtitle.h"
#include "translation.h"
#include "update.h"
#include "video.h"

namespace parabolic::qt::views
{
    SettingsPage::SettingsPage(QWidget* parent)
        : QScrollArea{ parent },
          m_layout{ new QVBoxLayout() }
    {
        QWidget* content{ new QWidget() };
        m_layout->setContentsMargins(24, 20, 24, 20);
        m_layout->setSpacing(12);
        content->setLayout(m_layout);
        setFrameShape(QFrame::NoFrame);
        setWidgetResizable(true);
        setWidget(content);
        content->setBackgroundRole(QPalette::Base);
    }

    QCheckBox* SettingsPage::add_check(QVBoxLayout* layout)
    {
        QCheckBox* check{ new QCheckBox(layout->parentWidget()) };
        layout->addWidget(check);
        return check;
    }

    QCheckBox* SettingsPage::add_check(QVBoxLayout* layout, oclero::qlementine::Label*& description)
    {
        QCheckBox* check{ add_check(layout) };
        description = add_label(layout, oclero::qlementine::TextRole::Caption);
        description->setContentsMargins(24, 0, 0, 0);
        return check;
    }

    QComboBox* SettingsPage::add_combo(QVBoxLayout* layout, oclero::qlementine::Label*& label)
    {
        label = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, layout->parentWidget());
        QComboBox* combo{ new QComboBox(layout->parentWidget()) };
        QHBoxLayout* row{ new QHBoxLayout() };
        row->addWidget(label);
        row->addWidget(combo);
        row->addStretch();
        layout->addLayout(row);
        return combo;
    }

    QGroupBox* SettingsPage::add_group(QVBoxLayout* page, QVBoxLayout*& layout)
    {
        QGroupBox* group{ new QGroupBox(page->parentWidget()) };
        layout = new QVBoxLayout(group);
        page->addWidget(group);
        return group;
    }

    oclero::qlementine::Label* SettingsPage::add_label(QVBoxLayout* layout, oclero::qlementine::TextRole role)
    {
        oclero::qlementine::Label* label{ new oclero::qlementine::Label({}, role, layout->parentWidget()) };
        label->setWordWrap(true);
        layout->addWidget(label);
        return label;
    }

    QSpinBox* SettingsPage::add_spin(QVBoxLayout* layout, oclero::qlementine::Label*& label, int minimum, int maximum)
    {
        label = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, layout->parentWidget());
        QSpinBox* spin{ new QSpinBox(layout->parentWidget()) };
        spin->setRange(minimum, maximum);
        QHBoxLayout* row{ new QHBoxLayout() };
        row->addWidget(label);
        row->addWidget(spin);
        row->addStretch();
        layout->addLayout(row);
        return spin;
    }

    QString SettingsPage::labeled(const char* text)
    {
        return QString::fromStdString(application::State::instance().translator()._("{}:", text));
    }

    QVBoxLayout* SettingsPage::page_layout() const
    {
        return m_layout;
    }

    void SettingsPage::set_items(QComboBox* combo, const QList<QString>& items)
    {
        const QSignalBlocker blocker{ combo };
        int index{ combo->currentIndex() };
        combo->clear();
        combo->addItems(items);
        combo->setCurrentIndex(std::max(index, 0));
    }

    PostprocessingArgumentDialog::PostprocessingArgumentDialog(const postprocessing::Argument& argument,
                                                               const std::vector<std::string>& taken_names,
                                                               QWidget* parent)
        : QDialog{ parent },
          m_taken_names{ taken_names },
          m_lbl_name{ new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, this) },
          m_edt_name{ new QLineEdit(this) },
          m_lbl_step{ new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, this) },
          m_cmb_step{ new QComboBox(this) },
          m_lbl_executable{ new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, this) },
          m_cmb_executable{ new QComboBox(this) },
          m_lbl_arguments{ new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, this) },
          m_edt_arguments{ new QLineEdit(this) },
          m_buttons{ new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this) }
    {
        setModal(true);
        setMinimumWidth(440);
        QFormLayout* layout{ new QFormLayout(this) };
        layout->addRow(m_lbl_name, m_edt_name);
        layout->addRow(m_lbl_step, m_cmb_step);
        layout->addRow(m_lbl_executable, m_cmb_executable);
        layout->addRow(m_lbl_arguments, m_edt_arguments);
        layout->addRow(m_buttons);
        translate();
        m_edt_name->setText(QString::fromStdString(argument.name()));
        m_cmb_step->setCurrentIndex(static_cast<int>(argument.step()));
        m_cmb_executable->setCurrentIndex(static_cast<int>(argument.executable()));
        m_edt_arguments->setText(QString::fromStdString(argument.args()));
        validate();
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed, this, &PostprocessingArgumentDialog::translate);
        connect(new QShortcut(QKeySequence::Close, this), &QShortcut::activated, this, &QDialog::reject);
        connect(m_edt_name, &QLineEdit::textChanged, this, &PostprocessingArgumentDialog::validate);
        connect(m_buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(m_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    }

    postprocessing::Argument PostprocessingArgumentDialog::argument() const
    {
        return { m_edt_name->text().trimmed().toStdString(),
                 static_cast<postprocessing::Step>(m_cmb_step->currentIndex()),
                 static_cast<postprocessing::Executable>(m_cmb_executable->currentIndex()),
                 m_edt_arguments->text().toStdString() };
    }

    void PostprocessingArgumentDialog::translate()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        const auto set_items = [](QComboBox* combo, const QList<QString>& items)
        {
            const QSignalBlocker blocker{ combo };
            int index{ combo->currentIndex() };
            combo->clear();
            combo->addItems(items);
            combo->setCurrentIndex(std::max(index, 0));
        };
        setWindowTitle(translator._("Postprocessing Argument"));
        m_lbl_name->setText(QString::fromStdString(translator._("{}:", translator._("Name"))));
        m_lbl_step->setText(QString::fromStdString(translator._("{}:", translator._("Post Processor"))));
        set_items(m_cmb_step, {
                                      translator._("None"),
                                      translator._("Merger"),
                                      translator._("Modify Chapters"),
                                      translator._("Split Chapters"),
                                      translator._("Extract Audio"),
                                      translator._("Video Remuxer"),
                                      translator._("Video Converter"),
                                      translator._("Metadata"),
                                      translator._("Embed Subtitle"),
                                      translator._("Embed Thumbnail"),
                                      translator._("Subtitles Converter"),
                                      translator._("Thumbnails Converter"),
                                      translator._("Fixup Stretched"),
                                      translator._("Fixup M4a"),
                                      translator._("Fixup M3u8"),
                                      translator._("Fixup Timestamp"),
                                      translator._("Fixup Duration"),
                              });
        m_lbl_executable->setText(QString::fromStdString(translator._("{}:", translator._("Executable"))));
        set_items(m_cmb_executable, {
                                            translator._("None"),
                                            translator._("AtomicParsley"),
                                            translator._("FFmpeg"),
                                            translator._("FFprobe"),
                                    });
        m_lbl_arguments->setText(QString::fromStdString(translator._("{}:", translator._("Argument"))));
    }

    void PostprocessingArgumentDialog::validate()
    {
        std::string name{ m_edt_name->text().trimmed().toStdString() };
        m_buttons->button(QDialogButtonBox::Ok)
                ->setEnabled(!name.empty() && !std::ranges::contains(m_taken_names, name));
    }

    SettingsGeneralPage::SettingsGeneralPage(QWidget* parent)
        : SettingsPage{ parent }
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        configuration::Configuration& configuration{
            application::State::instance().configuration(),
        };
        QVBoxLayout* user_interface_layout{ nullptr };
        m_grp_user_interface = add_group(page_layout(), user_interface_layout);
        m_cmb_theme = add_combo(user_interface_layout, m_lbl_theme);
        m_cmb_language = add_combo(user_interface_layout, m_lbl_language);
        QVBoxLayout* updates_layout{ nullptr };
        m_grp_updates = add_group(page_layout(), updates_layout);
        m_chk_automatic_updates = add_check(updates_layout);
        m_chk_preview_updates = add_check(updates_layout, m_dsc_preview_updates);
        QVBoxLayout* history_layout{ nullptr };
        m_grp_history = add_group(page_layout(), history_layout);
        m_cmb_history_length = add_combo(history_layout, m_lbl_history_length);
        page_layout()->addStretch();
        m_languages = translator.available_languages();
        m_cmb_language->addItems(QList<QString>(static_cast<qsizetype>(m_languages.size()) + 1));
        translate();
        const std::vector<std::string>::iterator language_it{
            std::ranges::find(m_languages, configuration.translation_options().language())
        };
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
                &helpers::TranslationNotifier::translation_changed, this, &SettingsGeneralPage::translate);
        connect(m_cmb_theme, &QComboBox::currentIndexChanged, this, &SettingsGeneralPage::change_theme);
        connect(m_cmb_language, &QComboBox::currentIndexChanged, this, &SettingsGeneralPage::change_language);
        connect(m_chk_automatic_updates, &QCheckBox::toggled, this, &SettingsGeneralPage::save);
        connect(m_chk_preview_updates, &QCheckBox::toggled, this, &SettingsGeneralPage::save);
        connect(m_cmb_history_length, &QComboBox::currentIndexChanged, this, &SettingsGeneralPage::save);
    }

    void SettingsGeneralPage::change_language()
    {
        configuration::Configuration& configuration{
            application::State::instance().configuration(),
        };
        translation::Options options{ configuration.translation_options() };
        options.set_language(m_cmb_language->currentIndex() == 0
                                     ? translation::Translator::follow_system_language()
                                     : m_languages[static_cast<std::size_t>(m_cmb_language->currentIndex()) - 1]);
        configuration.set_translation_options(options);
        qobject_cast<App*>(QApplication::instance())->translation_notifier()->translation_changed();
    }

    void SettingsGeneralPage::change_theme()
    {
        oclero::qlementine::ThemeManager* theme_manager{
            qobject_cast<App*>(QApplication::instance())->theme_manager(),
        };
        configuration::Theme theme{
            static_cast<configuration::Theme>(m_cmb_theme->currentIndex()),
        };
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
            theme_manager->setCurrentTheme(
                    QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ? "Legacy Dark" : "Light");
            break;
        }
    }

    void SettingsGeneralPage::save()
    {
        configuration::Configuration& configuration{
            application::State::instance().configuration(),
        };
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
        m_grp_user_interface->setTitle(translator._("User Interface"));
        m_lbl_theme->setText(labeled(translator._("Theme")));
        set_items(m_cmb_theme, {
                                       translator._p("Theme", "Light"),
                                       translator._p("Theme", "Dark"),
                                       translator._p("Theme", "System"),
                               });
        m_lbl_language->setText(labeled(translator._("Translation Language")));
        m_cmb_language->setItemText(0, translator._p("Language", "System"));
        std::string selected_language{
            m_cmb_language->currentIndex() > 0
                    ? m_languages[static_cast<std::size_t>(m_cmb_language->currentIndex()) - 1]
                    : std::string{}
        };
        std::vector<std::pair<QString, std::string>> languages;
        languages.reserve(m_languages.size());
        for (const std::string& language : m_languages)
        {
            std::array<UChar, 128> name{};
            UErrorCode status{ U_ZERO_ERROR };
            int32_t length{ uloc_getDisplayName(language.c_str(), translator.language().c_str(), name.data(),
                                                static_cast<int32_t>(name.size()), &status) };
            languages.emplace_back(U_SUCCESS(status) && length > 0 ? QString::fromUtf16(name.data(), length)
                                                                   : QString::fromStdString(language),
                                   language);
        }
        std::ranges::sort(languages,
                          [](const std::pair<QString, std::string>& a, const std::pair<QString, std::string>& b)
        {
            return a.first.localeAwareCompare(b.first) < 0;
        });
        const QSignalBlocker language_blocker{ m_cmb_language };
        for (std::size_t i{ 0 }; i < languages.size(); i++)
        {
            m_languages[i] = languages[i].second;
            m_cmb_language->setItemText(static_cast<int>(i) + 1, languages[i].first);
        }
        const std::vector<std::string>::iterator selected_it{ std::ranges::find(m_languages, selected_language) };
        m_cmb_language->setCurrentIndex(selected_it == m_languages.end() || selected_language.empty()
                                                ? 0
                                                : static_cast<int>(selected_it - m_languages.begin()) + 1);
        m_grp_updates->setTitle(translator._("Updates"));
        m_chk_automatic_updates->setText(translator._("Automatically Check for Updates"));
        m_chk_preview_updates->setText(translator._("Allow Preview Updates"));
        m_dsc_preview_updates->setText(
                translator._("Update the app and dependencies, such as yt-dlp, to beta versions."));
        m_grp_history->setTitle(translator._("History"));
        m_lbl_history_length->setText(labeled(translator._("Keep Download History For")));
        set_items(m_cmb_history_length, {
                                                translator._("Never"),
                                                translator._("1 Day"),
                                                translator._("1 Week"),
                                                translator._("1 Month"),
                                                translator._("3 Months"),
                                                translator._("6 Months"),
                                                translator._("1 Year"),
                                                translator._("Forever"),
                                        });
    }

    SettingsDiscoveryPage::SettingsDiscoveryPage(QWidget* parent)
        : SettingsPage{ parent }
    {
        configuration::Configuration& configuration{
            application::State::instance().configuration(),
        };
        QVBoxLayout* process_layout{ nullptr };
        m_grp_process = add_group(page_layout(), process_layout);
        m_chk_media_id = add_check(process_layout, m_dsc_media_id);
        m_chk_limit_characters = add_check(process_layout, m_dsc_limit_characters);
        m_chk_super_resolutions = add_check(process_layout, m_dsc_super_resolutions);
        m_chk_auto_generated_subtitles = add_check(process_layout);
        QVBoxLayout* preferences_layout{ nullptr };
        m_grp_preferences = add_group(page_layout(), preferences_layout);
        m_dsc_preferences = add_label(preferences_layout, oclero::qlementine::TextRole::Caption);
        m_cmb_audio_codec = add_combo(preferences_layout, m_lbl_audio_codec);
        m_cmb_video_codec = add_combo(preferences_layout, m_lbl_video_codec);
        m_cmb_subtitle_format = add_combo(preferences_layout, m_lbl_subtitle_format);
        m_cmb_frame_rate = add_combo(preferences_layout, m_lbl_frame_rate);
        QVBoxLayout* arguments_layout{ nullptr };
        m_grp_arguments = add_group(page_layout(), arguments_layout);
        m_dsc_arguments = add_label(arguments_layout, oclero::qlementine::TextRole::Caption);
        m_edt_arguments = new QLineEdit(arguments_layout->parentWidget());
        arguments_layout->addWidget(m_edt_arguments);
        page_layout()->addStretch();
        translate();
        const discovery::Options options{ configuration.discovery_options() };
        m_chk_media_id->setChecked(options.include_media_id_in_title());
        m_chk_limit_characters->setChecked(options.limit_characters());
        m_chk_super_resolutions->setChecked(options.include_super_resolutions());
        m_chk_auto_generated_subtitles->setChecked(options.include_auto_generated_subtitles());
        m_cmb_audio_codec->setCurrentIndex(static_cast<int>(options.preferred_audio_codec()));
        m_cmb_video_codec->setCurrentIndex(static_cast<int>(options.preferred_video_codec()));
        m_cmb_subtitle_format->setCurrentIndex(static_cast<int>(options.preferred_subtitle_format()));
        m_cmb_frame_rate->setCurrentIndex(static_cast<int>(options.preferred_frame_rate()));
        m_edt_arguments->setText(QString::fromStdString(options.ytdlp_arguments()));
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed, this, &SettingsDiscoveryPage::translate);
        connect(m_chk_media_id, &QCheckBox::toggled, this, &SettingsDiscoveryPage::save);
        connect(m_chk_limit_characters, &QCheckBox::toggled, this, &SettingsDiscoveryPage::save);
        connect(m_chk_super_resolutions, &QCheckBox::toggled, this, &SettingsDiscoveryPage::save);
        connect(m_chk_auto_generated_subtitles, &QCheckBox::toggled, this, &SettingsDiscoveryPage::save);
        connect(m_cmb_audio_codec, &QComboBox::currentIndexChanged, this, &SettingsDiscoveryPage::save);
        connect(m_cmb_video_codec, &QComboBox::currentIndexChanged, this, &SettingsDiscoveryPage::save);
        connect(m_cmb_subtitle_format, &QComboBox::currentIndexChanged, this, &SettingsDiscoveryPage::save);
        connect(m_cmb_frame_rate, &QComboBox::currentIndexChanged, this, &SettingsDiscoveryPage::save);
        connect(m_edt_arguments, &QLineEdit::textChanged, this, &SettingsDiscoveryPage::save);
    }

    void SettingsDiscoveryPage::save()
    {
        configuration::Configuration& configuration{
            application::State::instance().configuration(),
        };
        discovery::Options options{ configuration.discovery_options() };
        options.set_include_media_id_in_title(m_chk_media_id->isChecked());
        options.set_limit_characters(m_chk_limit_characters->isChecked());
        options.set_include_super_resolutions(m_chk_super_resolutions->isChecked());
        options.set_include_auto_generated_subtitles(m_chk_auto_generated_subtitles->isChecked());
        options.set_preferred_audio_codec(static_cast<audio::Codec>(m_cmb_audio_codec->currentIndex()));
        options.set_preferred_video_codec(static_cast<video::Codec>(m_cmb_video_codec->currentIndex()));
        options.set_preferred_subtitle_format(static_cast<subtitle::Type>(m_cmb_subtitle_format->currentIndex()));
        options.set_preferred_frame_rate(static_cast<video::FrameRate>(m_cmb_frame_rate->currentIndex()));
        options.set_ytdlp_arguments(m_edt_arguments->text().toStdString());
        configuration.set_discovery_options(options);
    }

    void SettingsDiscoveryPage::translate()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        m_grp_process->setTitle(translator._("Process"));
        m_chk_media_id->setText(translator._("Include Media Id in Title"));
        m_dsc_media_id->setText(translator._("Add the media entry's yt-dlp id to its default title."));
        m_chk_limit_characters->setText(translator._("Limit Filename Characters"));
        m_dsc_limit_characters->setText(
                translator._("Restrict filename characters to those supported only by Windows."));
        m_chk_super_resolutions->setText(translator._("Show Super Resolution Formats"));
        m_dsc_super_resolutions->setText(translator._(
                "Super resolutions are resolutions that have been enhanced with artificial intelligence."));
        m_chk_auto_generated_subtitles->setText(translator._("Show Auto-Generated Subtitles"));
        m_grp_preferences->setTitle(translator._("Preferences"));
        m_dsc_preferences->setText(translator._("Prefer these codecs and options when showing formats for download."));
        m_lbl_audio_codec->setText(labeled(translator._("Audio Codec")));
        set_items(m_cmb_audio_codec, {
                                             translator._("Any"),
                                             translator._("FLAC"),
                                             translator._("WAV"),
                                             translator._("Opus"),
                                             translator._("AAC"),
                                             translator._("MP4A"),
                                             translator._("MP3"),
                                     });
        m_lbl_video_codec->setText(labeled(translator._("Video Codec")));
        set_items(m_cmb_video_codec, {
                                             translator._("Any"),
                                             translator._("H.265"),
                                             translator._("VP9"),
                                             translator._("AV1"),
                                             translator._("H.264"),
                                     });
        m_lbl_subtitle_format->setText(labeled(translator._("Subtitle Format")));
        set_items(m_cmb_subtitle_format, {
                                                 translator._("Any"),
                                                 translator._("VTT"),
                                                 translator._("SRT"),
                                                 translator._("ASS"),
                                                 translator._("LRC"),
                                         });
        m_lbl_frame_rate->setText(labeled(translator._("Frame Rate")));
        set_items(m_cmb_frame_rate, {
                                            translator._("Any"),
                                            translator._("24 FPS"),
                                            translator._("30 FPS"),
                                            translator._("60 FPS"),
                                    });
        m_grp_arguments->setTitle(translator._("Arguments"));
        m_dsc_arguments->setText(
                translator._("These arguments will be appended to yt-dlp during the discovery phase."));
    }

    SettingsDownloadsPage::SettingsDownloadsPage(QWidget* parent)
        : SettingsPage{ parent }
    {
        configuration::Configuration& configuration{
            application::State::instance().configuration(),
        };
        QVBoxLayout* process_layout{ nullptr };
        m_grp_process = add_group(page_layout(), process_layout);
        m_spn_max_downloads = add_spin(process_layout, m_lbl_max_downloads, 1, 10);
        m_chk_overwrite_existing_files = add_check(process_layout);
        m_chk_prevent_suspend = add_check(process_layout);
        QVBoxLayout* aria_layout{ nullptr };
        m_grp_aria = add_group(page_layout(), aria_layout);
        m_dsc_aria = add_label(aria_layout, oclero::qlementine::TextRole::Caption);
        m_chk_use_aria = add_check(aria_layout);
        m_btn_learn_more = new QPushButton(aria_layout->parentWidget());
        m_btn_learn_more->setFlat(true);
        QHBoxLayout* learn_more_row{ new QHBoxLayout() };
        learn_more_row->addWidget(m_btn_learn_more);
        learn_more_row->addStretch();
        aria_layout->addLayout(learn_more_row);
        m_spn_aria_connections = add_spin(aria_layout, m_lbl_aria_connections, 1, 16);
        m_spn_aria_split_size = add_spin(aria_layout, m_lbl_aria_split_size, 1, 1024);
        QVBoxLayout* arguments_layout{ nullptr };
        m_grp_arguments = add_group(page_layout(), arguments_layout);
        m_dsc_arguments = add_label(arguments_layout, oclero::qlementine::TextRole::Caption);
        m_edt_arguments = new QLineEdit(arguments_layout->parentWidget());
        arguments_layout->addWidget(m_edt_arguments);
        page_layout()->addStretch();
        translate();
        const download::Options download_options{ configuration.download_options() };
        const aria2::Options aria2_options{ configuration.aria2_options() };
        m_spn_max_downloads->setValue(download_options.max_number_of_active_downloads());
        m_chk_overwrite_existing_files->setChecked(download_options.overwrite_existing_files());
        m_chk_prevent_suspend->setChecked(download_options.prevent_suspend());
        m_chk_use_aria->setChecked(aria2_options.use_aria());
        m_spn_aria_connections->setValue(aria2_options.max_connections_per_server());
        m_spn_aria_connections->setEnabled(aria2_options.use_aria());
        m_spn_aria_split_size->setValue(aria2_options.min_split_size());
        m_spn_aria_split_size->setEnabled(aria2_options.use_aria());
        m_edt_arguments->setText(QString::fromStdString(download_options.ytdlp_arguments()));
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed, this, &SettingsDownloadsPage::translate);
        connect(m_spn_max_downloads, &QSpinBox::valueChanged, this, &SettingsDownloadsPage::save);
        connect(m_chk_overwrite_existing_files, &QCheckBox::toggled, this, &SettingsDownloadsPage::save);
        connect(m_chk_prevent_suspend, &QCheckBox::toggled, this, &SettingsDownloadsPage::save);
        connect(m_chk_use_aria, &QCheckBox::toggled, this, [this](bool checked)
        {
            m_spn_aria_connections->setEnabled(checked);
            m_spn_aria_split_size->setEnabled(checked);
        });
        connect(m_chk_use_aria, &QCheckBox::toggled, this, &SettingsDownloadsPage::save);
        connect(m_btn_learn_more, &QPushButton::clicked, this, []
        {
            QDesktopServices::openUrl(QUrl("https://aria2.github.io"));
        });
        connect(m_spn_aria_connections, &QSpinBox::valueChanged, this, &SettingsDownloadsPage::save);
        connect(m_spn_aria_split_size, &QSpinBox::valueChanged, this, &SettingsDownloadsPage::save);
        connect(m_edt_arguments, &QLineEdit::textChanged, this, &SettingsDownloadsPage::save);
    }

    void SettingsDownloadsPage::save()
    {
        configuration::Configuration& configuration{
            application::State::instance().configuration(),
        };
        download::Options download_options{ configuration.download_options() };
        download_options.set_max_number_of_active_downloads(m_spn_max_downloads->value());
        download_options.set_overwrite_existing_files(m_chk_overwrite_existing_files->isChecked());
        download_options.set_prevent_suspend(m_chk_prevent_suspend->isChecked());
        download_options.set_ytdlp_arguments(m_edt_arguments->text().toStdString());
        configuration.set_download_options(download_options);
        aria2::Options aria2_options{ configuration.aria2_options() };
        aria2_options.set_use_aria(m_chk_use_aria->isChecked());
        aria2_options.set_max_connections_per_server(m_spn_aria_connections->value());
        aria2_options.set_min_split_size(m_spn_aria_split_size->value());
        configuration.set_aria2_options(aria2_options);
    }

    void SettingsDownloadsPage::translate()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        m_grp_process->setTitle(translator._("Process"));
        m_lbl_max_downloads->setText(labeled(translator._("Maximum Active Downloads")));
        m_chk_overwrite_existing_files->setText(translator._("Overwrite Existing Files"));
        m_chk_prevent_suspend->setText(translator._("Prevent Sleep During Downloads"));
        m_grp_aria->setTitle(translator._("aria2"));
        m_dsc_aria->setText(translator._("An alternative downloader that may be faster in some regions compared to "
                                         "yt-dlp's native downloader."));
        m_chk_use_aria->setText(translator._("Use aria2"));
        m_btn_learn_more->setText(translator._("Learn More…"));
        m_lbl_aria_connections->setText(labeled(translator._("Maximum Connections Per Server")));
        m_lbl_aria_split_size->setText(labeled(translator._("Minimum Split Size (MiB)")));
        m_grp_arguments->setTitle(translator._("Arguments"));
        m_dsc_arguments->setText(translator._("These arguments will be appended to yt-dlp during the download phase."));
    }

    SettingsExtractorPage::SettingsExtractorPage(QWidget* parent)
        : SettingsPage{ parent }
    {
        configuration::Configuration& configuration{
            application::State::instance().configuration(),
        };
        QVBoxLayout* process_layout{ nullptr };
        m_grp_process = add_group(page_layout(), process_layout);
        m_chk_part_files = add_check(process_layout, m_dsc_part_files);
        m_chk_sponsor_block = add_check(process_layout, m_dsc_sponsor_block);
        m_chk_translate_metadata = add_check(process_layout, m_dsc_translate_metadata);
        QVBoxLayout* speed_layout{ nullptr };
        m_grp_speed = add_group(page_layout(), speed_layout);
        m_chk_speed_limit = add_check(speed_layout);
        m_spn_speed_limit = add_spin(speed_layout, m_lbl_speed_limit, 512, 10240);
        QVBoxLayout* proxy_layout{ nullptr };
        m_grp_proxy = add_group(page_layout(), proxy_layout);
        m_lbl_proxy_url =
                new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, proxy_layout->parentWidget());
        m_edt_proxy_url = new QLineEdit(proxy_layout->parentWidget());
        QHBoxLayout* proxy_row{ new QHBoxLayout() };
        proxy_row->addWidget(m_lbl_proxy_url);
        proxy_row->addWidget(m_edt_proxy_url, 1);
        proxy_layout->addLayout(proxy_row);
        QVBoxLayout* cookies_layout{ nullptr };
        m_grp_cookies = add_group(page_layout(), cookies_layout);
        m_cmb_cookies_browser = add_combo(cookies_layout, m_lbl_cookies_browser);
        m_lbl_cookies_file = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default,
                                                           cookies_layout->parentWidget());
        m_edt_cookies_path = new QLineEdit(cookies_layout->parentWidget());
        m_edt_cookies_path->setReadOnly(true);
        m_btn_cookies_browse = new QPushButton(cookies_layout->parentWidget());
        m_btn_cookies_clear = new QPushButton(cookies_layout->parentWidget());
        QHBoxLayout* cookies_row{ new QHBoxLayout() };
        cookies_row->addWidget(m_lbl_cookies_file);
        cookies_row->addWidget(m_edt_cookies_path, 1);
        cookies_row->addWidget(m_btn_cookies_browse);
        cookies_row->addWidget(m_btn_cookies_clear);
        cookies_layout->addLayout(cookies_row);
        page_layout()->addStretch();
        translate();
        const extractor::Options options{ configuration.extractor_options() };
        m_chk_part_files->setChecked(options.use_part_files());
        m_chk_sponsor_block->setChecked(options.youtube_sponsor_block());
        m_chk_translate_metadata->setChecked(options.translate_metadata_and_chapters());
        m_chk_speed_limit->setChecked(options.speed_limit().has_value());
        m_spn_speed_limit->setValue(options.speed_limit().value_or(1024));
        m_spn_speed_limit->setEnabled(options.speed_limit().has_value());
        m_edt_proxy_url->setText(QString::fromStdString(options.proxy_url()));
        m_cmb_cookies_browser->setCurrentIndex(static_cast<int>(options.cookies_browser()));
        m_edt_cookies_path->setText(QString::fromStdString(options.cookies_path()));
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed, this, &SettingsExtractorPage::translate);
        connect(m_chk_part_files, &QCheckBox::toggled, this, &SettingsExtractorPage::save);
        connect(m_chk_sponsor_block, &QCheckBox::toggled, this, &SettingsExtractorPage::save);
        connect(m_chk_translate_metadata, &QCheckBox::toggled, this, &SettingsExtractorPage::save);
        connect(m_chk_speed_limit, &QCheckBox::toggled, this, [this](bool checked)
        {
            m_spn_speed_limit->setEnabled(checked);
        });
        connect(m_chk_speed_limit, &QCheckBox::toggled, this, &SettingsExtractorPage::save);
        connect(m_spn_speed_limit, &QSpinBox::valueChanged, this, &SettingsExtractorPage::save);
        connect(m_edt_proxy_url, &QLineEdit::textChanged, this, &SettingsExtractorPage::save);
        connect(m_cmb_cookies_browser, &QComboBox::currentIndexChanged, this, &SettingsExtractorPage::save);
        connect(m_edt_cookies_path, &QLineEdit::textChanged, this, &SettingsExtractorPage::save);
        connect(m_btn_cookies_browse, &QPushButton::clicked, this, &SettingsExtractorPage::browse_cookies_file);
        connect(m_btn_cookies_clear, &QPushButton::clicked, m_edt_cookies_path, &QLineEdit::clear);
    }

    void SettingsExtractorPage::browse_cookies_file()
    {
        QString path{
            QFileDialog::getOpenFileName(this, application::State::instance().translator()._("Cookies File")),
        };
        if (!path.isEmpty())
        {
            m_edt_cookies_path->setText(path);
        }
    }

    void SettingsExtractorPage::save()
    {
        configuration::Configuration& configuration{
            application::State::instance().configuration(),
        };
        extractor::Options options{ configuration.extractor_options() };
        options.set_use_part_files(m_chk_part_files->isChecked());
        options.set_youtube_sponsor_block(m_chk_sponsor_block->isChecked());
        options.set_translate_metadata_and_chapters(m_chk_translate_metadata->isChecked());
        options.set_speed_limit(m_chk_speed_limit->isChecked() ? std::optional<int>{ m_spn_speed_limit->value() }
                                                               : std::nullopt);
        options.set_proxy_url(m_edt_proxy_url->text().toStdString());
        options.set_cookies_browser(static_cast<cookies::Browser>(m_cmb_cookies_browser->currentIndex()));
        options.set_cookies_path(m_edt_cookies_path->text().toStdString());
        configuration.set_extractor_options(options);
    }

    void SettingsExtractorPage::translate()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        m_grp_process->setTitle(translator._("Process"));
        m_chk_part_files->setText(translator._("Use Part Files"));
        m_dsc_part_files->setText(
                translator._("Download media in separate .part files instead of directly into the output file."));
        m_chk_sponsor_block->setText(translator._("Enable SponsorBlock for YouTube"));
        m_dsc_sponsor_block->setText(translator._("Try to remove sponsored segments from videos."));
        m_chk_translate_metadata->setText(translator._("Translate Metadata and Chapters"));
        m_dsc_translate_metadata->setText(translator._("Automatically translate embedded metadata and chapters using "
                                                       "the app's set language on supported sites and media."));
        m_grp_speed->setTitle(translator._("Speed"));
        m_chk_speed_limit->setText(translator._("Limit Download Speed"));
        m_lbl_speed_limit->setText(labeled(translator._("Speed Limit (KiB/s)")));
        m_grp_proxy->setTitle(translator._("Proxy"));
        m_lbl_proxy_url->setText(labeled(translator._("URL")));
        m_grp_cookies->setTitle(translator._("Cookies"));
        m_lbl_cookies_browser->setText(labeled(translator._("Browser")));
        set_items(m_cmb_cookies_browser, {
                                                 translator._("None"),
                                                 translator._("Brave"),
                                                 translator._("Chrome"),
                                                 translator._("Chromium"),
                                                 translator._("Edge"),
                                                 translator._("Firefox"),
                                                 translator._("Opera"),
                                                 translator._("Vivaldi"),
                                                 translator._("Whale"),
                                         });
        m_lbl_cookies_file->setText(labeled(translator._("File")));
        m_btn_cookies_browse->setText(translator._("Browse…"));
        m_btn_cookies_clear->setText(translator._("Clear"));
    }

    SettingsPostprocessingPage::SettingsPostprocessingPage(QWidget* parent)
        : SettingsPage{ parent }
    {
        configuration::Configuration& configuration{
            application::State::instance().configuration(),
        };
        QVBoxLayout* metadata_layout{ nullptr };
        m_grp_metadata = add_group(page_layout(), metadata_layout);
        m_chk_embed_metadata = add_check(metadata_layout);
        m_chk_remove_source_data = add_check(metadata_layout, m_dsc_remove_source_data);
        QVBoxLayout* thumbnails_layout{ nullptr };
        m_grp_thumbnails = add_group(page_layout(), thumbnails_layout);
        m_chk_embed_thumbnails = add_check(thumbnails_layout, m_dsc_embed_thumbnails);
        m_chk_crop_audio_thumbnails = add_check(thumbnails_layout, m_dsc_crop_audio_thumbnails);
        QVBoxLayout* chapters_layout{ nullptr };
        m_grp_chapters = add_group(page_layout(), chapters_layout);
        m_chk_embed_chapters = add_check(chapters_layout);
        QVBoxLayout* subtitles_layout{ nullptr };
        m_grp_subtitles = add_group(page_layout(), subtitles_layout);
        m_chk_embed_subtitles = add_check(subtitles_layout, m_dsc_embed_subtitles);
        QVBoxLayout* ffmpeg_layout{ nullptr };
        m_grp_ffmpeg = add_group(page_layout(), ffmpeg_layout);
        m_spn_threads = add_spin(ffmpeg_layout, m_lbl_threads, 1,
                                 static_cast<int>(std::max(1U, std::thread::hardware_concurrency())));
        m_dsc_threads = add_label(ffmpeg_layout, oclero::qlementine::TextRole::Caption);
        QVBoxLayout* arguments_layout{ nullptr };
        m_grp_arguments = add_group(page_layout(), arguments_layout);
        m_dsc_arguments = add_label(arguments_layout, oclero::qlementine::TextRole::Caption);
        m_btn_add = new QToolButton(arguments_layout->parentWidget());
        m_btn_add->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(
                oclero::qlementine::icons::iconPath(oclero::qlementine::icons::Icons16::Action_Plus)));
        m_btn_edit = new QToolButton(arguments_layout->parentWidget());
        m_btn_edit->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(
                oclero::qlementine::icons::iconPath(oclero::qlementine::icons::Icons16::Misc_Pen)));
        m_btn_remove = new QToolButton(arguments_layout->parentWidget());
        m_btn_remove->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(
                oclero::qlementine::icons::iconPath(oclero::qlementine::icons::Icons16::Action_Minus)));
        QHBoxLayout* buttons{ new QHBoxLayout() };
        buttons->addWidget(m_btn_add);
        buttons->addWidget(m_btn_edit);
        buttons->addWidget(m_btn_remove);
        buttons->addStretch();
        arguments_layout->addLayout(buttons);
        m_lst_arguments = new QListWidget(arguments_layout->parentWidget());
        m_lst_arguments->setMinimumHeight(120);
        arguments_layout->addWidget(m_lst_arguments);
        page_layout()->addStretch();
        translate();
        const postprocessing::Options options{ configuration.postprocessing_options() };
        m_chk_embed_metadata->setChecked(options.embed_metadata());
        m_chk_remove_source_data->setChecked(options.remove_source_data());
        m_chk_remove_source_data->setEnabled(options.embed_metadata());
        m_chk_embed_thumbnails->setChecked(options.embed_thumbnails());
        m_chk_crop_audio_thumbnails->setChecked(options.crop_audio_thumbnails());
        m_chk_crop_audio_thumbnails->setEnabled(options.embed_thumbnails());
        m_chk_embed_chapters->setChecked(options.embed_chapters());
        m_chk_embed_subtitles->setChecked(options.embed_subtitles());
        m_spn_threads->setValue(options.threads());
        m_arguments = options.arguments();
        for (const postprocessing::Argument& argument : m_arguments)
        {
            m_lst_arguments->addItem(QString::fromStdString(argument.name()));
        }
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed, this, &SettingsPostprocessingPage::translate);
        connect(m_chk_embed_metadata, &QCheckBox::toggled, this, [this](bool checked)
        {
            m_chk_remove_source_data->setEnabled(checked);
        });
        connect(m_chk_embed_metadata, &QCheckBox::toggled, this, &SettingsPostprocessingPage::save);
        connect(m_chk_remove_source_data, &QCheckBox::toggled, this, &SettingsPostprocessingPage::save);
        connect(m_chk_embed_thumbnails, &QCheckBox::toggled, this, [this](bool checked)
        {
            m_chk_crop_audio_thumbnails->setEnabled(checked);
        });
        connect(m_chk_embed_thumbnails, &QCheckBox::toggled, this, &SettingsPostprocessingPage::save);
        connect(m_chk_crop_audio_thumbnails, &QCheckBox::toggled, this, &SettingsPostprocessingPage::save);
        connect(m_chk_embed_chapters, &QCheckBox::toggled, this, &SettingsPostprocessingPage::save);
        connect(m_chk_embed_subtitles, &QCheckBox::toggled, this, &SettingsPostprocessingPage::save);
        connect(m_spn_threads, &QSpinBox::valueChanged, this, &SettingsPostprocessingPage::save);
        connect(m_btn_add, &QToolButton::clicked, this, &SettingsPostprocessingPage::add_argument);
        connect(m_btn_edit, &QToolButton::clicked, this, &SettingsPostprocessingPage::edit_argument);
        connect(m_btn_remove, &QToolButton::clicked, this, &SettingsPostprocessingPage::remove_argument);
        connect(m_lst_arguments, &QListWidget::itemDoubleClicked, this, &SettingsPostprocessingPage::edit_argument);
    }

    void SettingsPostprocessingPage::add_argument()
    {
        std::vector<std::string> names;
        names.reserve(m_arguments.size());
        for (const postprocessing::Argument& argument : m_arguments)
        {
            names.push_back(argument.name());
        }
        PostprocessingArgumentDialog dialog{ { "", postprocessing::Step::None, postprocessing::Executable::None, "" },
                                             names,
                                             this };
        if (dialog.exec() != QDialog::Accepted)
        {
            return;
        }
        m_arguments.push_back(dialog.argument());
        m_lst_arguments->addItem(QString::fromStdString(dialog.argument().name()));
        m_lst_arguments->setCurrentRow(m_lst_arguments->count() - 1);
        save();
    }

    void SettingsPostprocessingPage::edit_argument()
    {
        int row{ m_lst_arguments->currentRow() };
        if (row < 0)
        {
            return;
        }
        std::vector<std::string> names;
        for (std::size_t i{ 0 }; i < m_arguments.size(); i++)
        {
            if (std::cmp_not_equal(i, row))
            {
                names.push_back(m_arguments[i].name());
            }
        }
        PostprocessingArgumentDialog dialog{ m_arguments[static_cast<std::size_t>(row)], names, this };
        if (dialog.exec() != QDialog::Accepted)
        {
            return;
        }
        m_arguments[static_cast<std::size_t>(row)] = dialog.argument();
        m_lst_arguments->item(row)->setText(QString::fromStdString(dialog.argument().name()));
        save();
    }

    void SettingsPostprocessingPage::remove_argument()
    {
        int row{ m_lst_arguments->currentRow() };
        if (row < 0)
        {
            return;
        }
        const translation::Translator& translator{ application::State::instance().translator() };
        if (QMessageBox::question(this, translator._("Remove Argument"),
                                  QString::fromStdString(
                                          translator._("Are you sure you want to remove \"{}\"?",
                                                       m_arguments[static_cast<std::size_t>(row)].name()))) !=
            QMessageBox::Yes)
        {
            return;
        }
        m_arguments.erase(m_arguments.begin() + row);
        delete m_lst_arguments->takeItem(row);
        save();
    }

    void SettingsPostprocessingPage::save()
    {
        configuration::Configuration& configuration{
            application::State::instance().configuration(),
        };
        postprocessing::Options options{ configuration.postprocessing_options() };
        options.set_embed_metadata(m_chk_embed_metadata->isChecked());
        options.set_remove_source_data(m_chk_remove_source_data->isChecked());
        options.set_embed_thumbnails(m_chk_embed_thumbnails->isChecked());
        options.set_crop_audio_thumbnails(m_chk_crop_audio_thumbnails->isChecked());
        options.set_embed_chapters(m_chk_embed_chapters->isChecked());
        options.set_embed_subtitles(m_chk_embed_subtitles->isChecked());
        options.set_threads(m_spn_threads->value());
        options.set_arguments(m_arguments);
        configuration.set_postprocessing_options(options);
    }

    void SettingsPostprocessingPage::translate()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        m_grp_metadata->setTitle(translator._("Metadata"));
        m_chk_embed_metadata->setText(translator._("Embed Metadata"));
        m_chk_remove_source_data->setText(translator._("Remove Source Data"));
        m_dsc_remove_source_data->setText(
                translator._("Clear metadata fields containing identifying download information."));
        m_grp_thumbnails->setTitle(translator._("Thumbnails"));
        m_chk_embed_thumbnails->setText(translator._("Embed Thumbnails"));
        m_dsc_embed_thumbnails->setText(translator._("If the file type does not support embedding, the thumbnail will "
                                                     "be written to a separate image file."));
        m_chk_crop_audio_thumbnails->setText(translator._("Crop Audio Thumbnails"));
        m_dsc_crop_audio_thumbnails->setText(translator._("Crop thumbnails of audio files to squares."));
        m_grp_chapters->setTitle(translator._("Chapters"));
        m_chk_embed_chapters->setText(translator._("Embed Chapters"));
        m_grp_subtitles->setTitle(translator._("Subtitles"));
        m_chk_embed_subtitles->setText(translator._("Embed Subtitles"));
        m_dsc_embed_subtitles->setText(translator._(
                "If disabled or if embedding is not supported, downloaded subtitles will be saved to separate files."));
        m_grp_ffmpeg->setTitle(translator._("FFmpeg"));
        m_lbl_threads->setText(labeled(translator._("Threads")));
        m_dsc_threads->setText(translator._("Limit the number of threads used by ffmpeg."));
        m_grp_arguments->setTitle(translator._("Arguments"));
        m_dsc_arguments->setText(
                translator._("Arguments will be shown for selection in the download discovery dialog."));
        m_btn_add->setToolTip(translator._("Add"));
        m_btn_edit->setToolTip(translator._("Edit"));
        m_btn_remove->setToolTip(translator._("Remove"));
    }

    SettingsDialog::SettingsDialog(QWidget* parent)
        : QDialog{ parent },
          m_navigation_view{ new oclero::qlementine::NavigationView(this) }
    {
        setModal(true);
        setFixedSize(700, 500);
        QVBoxLayout* layout{ new QVBoxLayout(this) };
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(m_navigation_view);
        m_navigation_view->sidebar()->setExpandedWidth(180);
        SettingsGeneralPage* general_page{
            new SettingsGeneralPage(m_navigation_view),
        };
        SettingsDiscoveryPage* discovery_page{ new SettingsDiscoveryPage(m_navigation_view) };
        SettingsDownloadsPage* downloads_page{ new SettingsDownloadsPage(m_navigation_view) };
        SettingsExtractorPage* extractor_page{ new SettingsExtractorPage(m_navigation_view) };
        SettingsPostprocessingPage* postprocessing_page{
            new SettingsPostprocessingPage(m_navigation_view),
        };
        m_navigation_view->addPage(general_page, {},
                                   oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                           oclero::qlementine::icons::Icons16::Navigation_Settings)));
        m_navigation_view->addPage(discovery_page, {},
                                   oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                           oclero::qlementine::icons::Icons16::Navigation_Search)));
        m_navigation_view->addPage(downloads_page, {},
                                   oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                           oclero::qlementine::icons::Icons16::Action_Download)));
        m_navigation_view->addPage(extractor_page, {},
                                   oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                           oclero::qlementine::icons::Icons16::File_Plugin)));
        m_navigation_view->addPage(postprocessing_page, {},
                                   oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                           oclero::qlementine::icons::Icons16::Action_Build)));
        translate();
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed, this, &SettingsDialog::translate);
        connect(new QShortcut(QKeySequence::Close, this), &QShortcut::activated, this, &QDialog::reject);
    }

    void SettingsDialog::translate()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        setWindowTitle(translator._("Settings"));
        m_navigation_view->sidebar()->setItemText(0, translator._p("Settings", "General"));
        m_navigation_view->sidebar()->setItemText(1, translator._p("Settings", "Discovery"));
        m_navigation_view->sidebar()->setItemText(2, translator._p("Settings", "Downloads"));
        m_navigation_view->sidebar()->setItemText(3, translator._p("Settings", "Extractor"));
        m_navigation_view->sidebar()->setItemText(4, translator._p("Settings", "Postprocessing"));
    }
}
