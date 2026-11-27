#include "views/settings_dialog.h"
#include <QApplication>
#include <QCheckBox>
#include <QCollator>
#include <QComboBox>
#include <QDesktopServices>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QFrame>
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
#include <QtCore/qtypes.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <oclero/qlementine/Common.hpp>
#include <oclero/qlementine/icons/Icons16.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/style/ThemeManager.hpp>
#include <oclero/qlementine/widgets/GroupBox.hpp>
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
        setWindowTitle(translator._("Postprocessing Argument"));
        m_lbl_name->setText(QString::fromStdString(translator._("{}:", translator._("Name"))));
        m_lbl_step->setText(QString::fromStdString(translator._("{}:", translator._("Post Processor"))));
        QSignalBlocker step_blocker{ m_cmb_step };
        int step_index{ m_cmb_step->currentIndex() };
        m_cmb_step->clear();
        m_cmb_step->addItems({
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
        m_cmb_step->setCurrentIndex(std::max(step_index, 0));
        m_lbl_executable->setText(QString::fromStdString(translator._("{}:", translator._("Executable"))));
        QSignalBlocker executable_blocker{ m_cmb_executable };
        int executable_index{ m_cmb_executable->currentIndex() };
        m_cmb_executable->clear();
        m_cmb_executable->addItems({
                translator._("None"),
                translator._("AtomicParsley"),
                translator._("FFmpeg"),
                translator._("FFprobe"),
        });
        m_cmb_executable->setCurrentIndex(std::max(executable_index, 0));
        m_lbl_arguments->setText(QString::fromStdString(translator._("{}:", translator._("Argument"))));
    }

    void PostprocessingArgumentDialog::validate()
    {
        std::string name{ m_edt_name->text().trimmed().toStdString() };
        m_buttons->button(QDialogButtonBox::Ok)
                ->setEnabled(!name.empty() && !std::ranges::contains(m_taken_names, name));
    }

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
        configuration::Configuration& configuration{
            application::State::instance().configuration(),
        };
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
        m_dsc_preview_updates = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_updates);
        m_dsc_preview_updates->setWordWrap(true);
        m_dsc_preview_updates->setContentsMargins(24, 0, 0, 0);
        updates_layout->addWidget(m_dsc_preview_updates);
        QFormLayout* history_layout{ new QFormLayout(m_grp_history) };
        history_layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
        layout->addWidget(m_grp_history);
        m_lbl_history_length = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_history);
        history_layout->addRow(m_lbl_history_length, m_cmb_history_length);
        layout->addStretch();
        m_cmb_language->addItems(QList<QString>(static_cast<qsizetype>(m_languages.size()) + 1));
        translate();
        std::vector<std::string>::iterator language_it{
            std::ranges::find(m_languages, configuration.translation_options().language()),
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
        m_lbl_theme->setText(QString::fromStdString(translator._("{}:", translator._("Theme"))));
        QSignalBlocker theme_blocker{ m_cmb_theme };
        int theme_index{ m_cmb_theme->currentIndex() };
        m_cmb_theme->clear();
        m_cmb_theme->addItems({
                translator._p("Theme", "Light"),
                translator._p("Theme", "Dark"),
                translator._p("Theme", "System"),
        });
        m_cmb_theme->setCurrentIndex(std::max(theme_index, 0));
        m_lbl_language->setText(QString::fromStdString(translator._("{}:", translator._("Translation Language"))));
        m_cmb_language->setItemText(0, translator._p("Language", "System"));
        std::string selected_language{
            m_cmb_language->currentIndex() > 0
                    ? m_languages[static_cast<std::size_t>(m_cmb_language->currentIndex()) - 1]
                    : std::string{},
        };
        std::vector<std::pair<QString, std::string>> languages;
        languages.reserve(m_languages.size());
        for (const std::string& language : m_languages)
        {
            std::array<UChar, 128> name{};
            UErrorCode status{ U_ZERO_ERROR };
            int32_t length{
                uloc_getDisplayName(language.c_str(), translator.language().c_str(), name.data(),
                                    static_cast<int32_t>(name.size()), &status),
            };
            languages.emplace_back(U_SUCCESS(status) != 0 && length > 0 ? QString::fromUtf16(name.data(), length)
                                                                        : QString::fromStdString(language),
                                   language);
        }
        QCollator collator;
        std::ranges::sort(languages, collator, &std::pair<QString, std::string>::first);
        QSignalBlocker language_blocker{ m_cmb_language };
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
        m_dsc_preview_updates->setText(
                translator._("Update the app and dependencies, such as yt-dlp, to beta versions."));
        m_grp_history->setTitle(translator._("History"));
        m_lbl_history_length->setText(
                QString::fromStdString(translator._("{}:", translator._("Keep Download History For"))));
        QSignalBlocker history_length_blocker{ m_cmb_history_length };
        int history_length_index{ m_cmb_history_length->currentIndex() };
        m_cmb_history_length->clear();
        m_cmb_history_length->addItems({
                translator._("Never"),
                translator._("1 Day"),
                translator._("1 Week"),
                translator._("1 Month"),
                translator._("3 Months"),
                translator._("6 Months"),
                translator._("1 Year"),
                translator._("Forever"),
        });
        m_cmb_history_length->setCurrentIndex(std::max(history_length_index, 0));
    }

    SettingsDiscoveryPage::SettingsDiscoveryPage(QWidget* parent)
        : QScrollArea{ parent },
          m_grp_process{ new oclero::qlementine::GroupBox(this) },
          m_chk_media_id{ new QCheckBox(m_grp_process) },
          m_chk_limit_characters{ new QCheckBox(m_grp_process) },
          m_chk_super_resolutions{ new QCheckBox(m_grp_process) },
          m_chk_auto_generated_subtitles{ new QCheckBox(m_grp_process) },
          m_grp_preferences{ new oclero::qlementine::GroupBox(this) },
          m_cmb_audio_codec{ new QComboBox(m_grp_preferences) },
          m_cmb_video_codec{ new QComboBox(m_grp_preferences) },
          m_cmb_subtitle_format{ new QComboBox(m_grp_preferences) },
          m_cmb_frame_rate{ new QComboBox(m_grp_preferences) },
          m_grp_arguments{ new oclero::qlementine::GroupBox(this) },
          m_edt_arguments{ new QLineEdit(m_grp_arguments) }
    {
        QWidget* content{ new QWidget() };
        QVBoxLayout* layout{ new QVBoxLayout(content) };
        layout->setContentsMargins(24, 20, 24, 20);
        layout->setSpacing(12);
        setFrameShape(QFrame::NoFrame);
        setWidgetResizable(true);
        setWidget(content);
        content->setBackgroundRole(QPalette::Base);
        configuration::Configuration& configuration{
            application::State::instance().configuration(),
        };
        QVBoxLayout* process_layout{ new QVBoxLayout(m_grp_process) };
        layout->addWidget(m_grp_process);
        process_layout->addWidget(m_chk_media_id);
        m_dsc_media_id = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_process);
        m_dsc_media_id->setWordWrap(true);
        m_dsc_media_id->setContentsMargins(24, 0, 0, 0);
        process_layout->addWidget(m_dsc_media_id);
        process_layout->addWidget(m_chk_limit_characters);
        m_dsc_limit_characters =
                new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_process);
        m_dsc_limit_characters->setWordWrap(true);
        m_dsc_limit_characters->setContentsMargins(24, 0, 0, 0);
        process_layout->addWidget(m_dsc_limit_characters);
        process_layout->addWidget(m_chk_super_resolutions);
        m_dsc_super_resolutions =
                new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_process);
        m_dsc_super_resolutions->setWordWrap(true);
        m_dsc_super_resolutions->setContentsMargins(24, 0, 0, 0);
        process_layout->addWidget(m_dsc_super_resolutions);
        process_layout->addWidget(m_chk_auto_generated_subtitles);
        QFormLayout* preferences_layout{ new QFormLayout(m_grp_preferences) };
        preferences_layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
        layout->addWidget(m_grp_preferences);
        m_lbl_audio_codec = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_preferences);
        preferences_layout->addRow(m_lbl_audio_codec, m_cmb_audio_codec);
        m_lbl_video_codec = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_preferences);
        preferences_layout->addRow(m_lbl_video_codec, m_cmb_video_codec);
        m_lbl_subtitle_format =
                new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_preferences);
        preferences_layout->addRow(m_lbl_subtitle_format, m_cmb_subtitle_format);
        m_lbl_frame_rate = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_preferences);
        preferences_layout->addRow(m_lbl_frame_rate, m_cmb_frame_rate);
        QVBoxLayout* arguments_layout{ new QVBoxLayout(m_grp_arguments) };
        layout->addWidget(m_grp_arguments);
        arguments_layout->addWidget(m_edt_arguments);
        layout->addStretch();
        translate();
        discovery::Options options{ configuration.discovery_options() };
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
        m_grp_preferences->setDescription(
                translator._("Prefer these codecs and options when showing formats for download."));
        m_lbl_audio_codec->setText(QString::fromStdString(translator._("{}:", translator._("Audio Codec"))));
        QSignalBlocker audio_codec_blocker{ m_cmb_audio_codec };
        int audio_codec_index{ m_cmb_audio_codec->currentIndex() };
        m_cmb_audio_codec->clear();
        m_cmb_audio_codec->addItems({
                translator._("Any"),
                translator._("FLAC"),
                translator._("WAV"),
                translator._("Opus"),
                translator._("AAC"),
                translator._("MP4A"),
                translator._("MP3"),
        });
        m_cmb_audio_codec->setCurrentIndex(std::max(audio_codec_index, 0));
        m_lbl_video_codec->setText(QString::fromStdString(translator._("{}:", translator._("Video Codec"))));
        QSignalBlocker video_codec_blocker{ m_cmb_video_codec };
        int video_codec_index{ m_cmb_video_codec->currentIndex() };
        m_cmb_video_codec->clear();
        m_cmb_video_codec->addItems({
                translator._("Any"),
                translator._("H.265"),
                translator._("VP9"),
                translator._("AV1"),
                translator._("H.264"),
        });
        m_cmb_video_codec->setCurrentIndex(std::max(video_codec_index, 0));
        m_lbl_subtitle_format->setText(QString::fromStdString(translator._("{}:", translator._("Subtitle Format"))));
        QSignalBlocker subtitle_format_blocker{ m_cmb_subtitle_format };
        int subtitle_format_index{ m_cmb_subtitle_format->currentIndex() };
        m_cmb_subtitle_format->clear();
        m_cmb_subtitle_format->addItems({
                translator._("Any"),
                translator._("VTT"),
                translator._("SRT"),
                translator._("ASS"),
                translator._("LRC"),
        });
        m_cmb_subtitle_format->setCurrentIndex(std::max(subtitle_format_index, 0));
        m_lbl_frame_rate->setText(QString::fromStdString(translator._("{}:", translator._("Frame Rate"))));
        QSignalBlocker frame_rate_blocker{ m_cmb_frame_rate };
        int frame_rate_index{ m_cmb_frame_rate->currentIndex() };
        m_cmb_frame_rate->clear();
        m_cmb_frame_rate->addItems({
                translator._("Any"),
                translator._("24 FPS"),
                translator._("30 FPS"),
                translator._("60 FPS"),
        });
        m_cmb_frame_rate->setCurrentIndex(std::max(frame_rate_index, 0));
        m_grp_arguments->setTitle(translator._("Arguments"));
        m_grp_arguments->setDescription(
                translator._("These arguments will be appended to yt-dlp during the discovery phase."));
    }

    SettingsDownloadsPage::SettingsDownloadsPage(QWidget* parent)
        : QScrollArea{ parent },
          m_grp_process{ new oclero::qlementine::GroupBox(this) },
          m_spn_max_downloads{ new QSpinBox(m_grp_process) },
          m_chk_overwrite_existing_files{ new QCheckBox(m_grp_process) },
          m_chk_prevent_suspend{ new QCheckBox(m_grp_process) },
          m_grp_aria{ new oclero::qlementine::GroupBox(this) },
          m_chk_use_aria{ new QCheckBox(m_grp_aria) },
          m_btn_learn_more{ new QToolButton(m_grp_aria) },
          m_spn_aria_connections{ new QSpinBox(m_grp_aria) },
          m_spn_aria_split_size{ new QSpinBox(m_grp_aria) },
          m_grp_arguments{ new oclero::qlementine::GroupBox(this) },
          m_edt_arguments{ new QLineEdit(m_grp_arguments) }
    {
        QWidget* content{ new QWidget() };
        QVBoxLayout* layout{ new QVBoxLayout(content) };
        layout->setContentsMargins(24, 20, 24, 20);
        layout->setSpacing(12);
        setFrameShape(QFrame::NoFrame);
        setWidgetResizable(true);
        setWidget(content);
        content->setBackgroundRole(QPalette::Base);
        configuration::Configuration& configuration{
            application::State::instance().configuration(),
        };
        QVBoxLayout* process_layout{ new QVBoxLayout(m_grp_process) };
        layout->addWidget(m_grp_process);
        m_lbl_max_downloads = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_process);
        m_spn_max_downloads->setRange(1, 10);
        QHBoxLayout* max_downloads_row{ new QHBoxLayout() };
        max_downloads_row->addWidget(m_lbl_max_downloads);
        max_downloads_row->addWidget(m_spn_max_downloads);
        max_downloads_row->addStretch();
        process_layout->addLayout(max_downloads_row);
        process_layout->addWidget(m_chk_overwrite_existing_files);
        process_layout->addWidget(m_chk_prevent_suspend);
        QVBoxLayout* aria_layout{ new QVBoxLayout(m_grp_aria) };
        layout->addWidget(m_grp_aria);
        m_btn_learn_more->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(
                oclero::qlementine::icons::iconPath(oclero::qlementine::icons::Icons16::Misc_Info)));
        QHBoxLayout* use_aria_row{ new QHBoxLayout() };
        use_aria_row->addWidget(m_chk_use_aria);
        use_aria_row->addWidget(m_btn_learn_more);
        use_aria_row->addStretch();
        aria_layout->addLayout(use_aria_row);
        QFormLayout* aria_form_layout{ new QFormLayout() };
        aria_form_layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
        aria_layout->addLayout(aria_form_layout);
        m_lbl_aria_connections = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_aria);
        m_spn_aria_connections->setRange(1, 16);
        aria_form_layout->addRow(m_lbl_aria_connections, m_spn_aria_connections);
        m_lbl_aria_split_size = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_aria);
        m_spn_aria_split_size->setRange(1, 1024);
        aria_form_layout->addRow(m_lbl_aria_split_size, m_spn_aria_split_size);
        QVBoxLayout* arguments_layout{ new QVBoxLayout(m_grp_arguments) };
        layout->addWidget(m_grp_arguments);
        arguments_layout->addWidget(m_edt_arguments);
        layout->addStretch();
        translate();
        download::Options download_options{ configuration.download_options() };
        aria2::Options aria2_options{ configuration.aria2_options() };
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
        connect(m_chk_use_aria, &QCheckBox::toggled, this, &SettingsDownloadsPage::save);
        connect(m_btn_learn_more, &QToolButton::clicked, this, &SettingsDownloadsPage::learn_more);
        connect(m_spn_aria_connections, &QSpinBox::valueChanged, this, &SettingsDownloadsPage::save);
        connect(m_spn_aria_split_size, &QSpinBox::valueChanged, this, &SettingsDownloadsPage::save);
        connect(m_edt_arguments, &QLineEdit::textChanged, this, &SettingsDownloadsPage::save);
    }

    void SettingsDownloadsPage::learn_more()
    {
        QDesktopServices::openUrl(QUrl("https://aria2.github.io"));
    }

    void SettingsDownloadsPage::save()
    {
        m_spn_aria_connections->setEnabled(m_chk_use_aria->isChecked());
        m_spn_aria_split_size->setEnabled(m_chk_use_aria->isChecked());
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
        m_lbl_max_downloads->setText(
                QString::fromStdString(translator._("{}:", translator._("Maximum Active Downloads"))));
        m_chk_overwrite_existing_files->setText(translator._("Overwrite Existing Files"));
        m_chk_prevent_suspend->setText(translator._("Prevent Sleep During Downloads"));
        m_grp_aria->setTitle(translator._("aria2"));
        m_grp_aria->setDescription(
                translator._("An alternative downloader that may be faster in some regions compared to "
                             "yt-dlp's native downloader."));
        m_chk_use_aria->setText(translator._("Use aria2"));
        m_btn_learn_more->setToolTip(translator._("Learn More"));
        m_lbl_aria_connections->setText(
                QString::fromStdString(translator._("{}:", translator._("Maximum Connections Per Server"))));
        m_lbl_aria_split_size->setText(
                QString::fromStdString(translator._("{}:", translator._("Minimum Split Size (MiB)"))));
        m_grp_arguments->setTitle(translator._("Arguments"));
        m_grp_arguments->setDescription(
                translator._("These arguments will be appended to yt-dlp during the download phase."));
    }

    SettingsExtractorPage::SettingsExtractorPage(QWidget* parent)
        : QScrollArea{ parent },
          m_grp_process{ new oclero::qlementine::GroupBox(this) },
          m_chk_part_files{ new QCheckBox(m_grp_process) },
          m_chk_sponsor_block{ new QCheckBox(m_grp_process) },
          m_chk_translate_metadata{ new QCheckBox(m_grp_process) },
          m_grp_speed{ new oclero::qlementine::GroupBox(this) },
          m_chk_speed_limit{ new QCheckBox(m_grp_speed) },
          m_spn_speed_limit{ new QSpinBox(m_grp_speed) },
          m_grp_proxy{ new oclero::qlementine::GroupBox(this) },
          m_edt_proxy_url{ new QLineEdit(m_grp_proxy) },
          m_grp_cookies{ new oclero::qlementine::GroupBox(this) },
          m_cmb_cookies_browser{ new QComboBox(m_grp_cookies) },
          m_edt_cookies_path{ new QLineEdit(m_grp_cookies) },
          m_btn_cookies_browse{ new QPushButton(m_grp_cookies) },
          m_btn_cookies_clear{ new QPushButton(m_grp_cookies) }
    {
        QWidget* content{ new QWidget() };
        QVBoxLayout* layout{ new QVBoxLayout(content) };
        layout->setContentsMargins(24, 20, 24, 20);
        layout->setSpacing(12);
        setFrameShape(QFrame::NoFrame);
        setWidgetResizable(true);
        setWidget(content);
        content->setBackgroundRole(QPalette::Base);
        configuration::Configuration& configuration{
            application::State::instance().configuration(),
        };
        QVBoxLayout* process_layout{ new QVBoxLayout(m_grp_process) };
        layout->addWidget(m_grp_process);
        process_layout->addWidget(m_chk_part_files);
        m_dsc_part_files = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_process);
        m_dsc_part_files->setWordWrap(true);
        m_dsc_part_files->setContentsMargins(24, 0, 0, 0);
        process_layout->addWidget(m_dsc_part_files);
        process_layout->addWidget(m_chk_sponsor_block);
        m_dsc_sponsor_block = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_process);
        m_dsc_sponsor_block->setWordWrap(true);
        m_dsc_sponsor_block->setContentsMargins(24, 0, 0, 0);
        process_layout->addWidget(m_dsc_sponsor_block);
        process_layout->addWidget(m_chk_translate_metadata);
        m_dsc_translate_metadata =
                new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_process);
        m_dsc_translate_metadata->setWordWrap(true);
        m_dsc_translate_metadata->setContentsMargins(24, 0, 0, 0);
        process_layout->addWidget(m_dsc_translate_metadata);
        QVBoxLayout* speed_layout{ new QVBoxLayout(m_grp_speed) };
        layout->addWidget(m_grp_speed);
        speed_layout->addWidget(m_chk_speed_limit);
        m_lbl_speed_limit = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_speed);
        m_spn_speed_limit->setRange(512, 10240);
        QHBoxLayout* speed_limit_row{ new QHBoxLayout() };
        speed_limit_row->addWidget(m_lbl_speed_limit);
        speed_limit_row->addWidget(m_spn_speed_limit);
        speed_limit_row->addStretch();
        speed_layout->addLayout(speed_limit_row);
        QFormLayout* proxy_layout{ new QFormLayout(m_grp_proxy) };
        proxy_layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        layout->addWidget(m_grp_proxy);
        m_lbl_proxy_url = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_proxy);
        proxy_layout->addRow(m_lbl_proxy_url, m_edt_proxy_url);
        QFormLayout* cookies_layout{ new QFormLayout(m_grp_cookies) };
        cookies_layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        layout->addWidget(m_grp_cookies);
        m_lbl_cookies_browser = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_cookies);
        cookies_layout->addRow(m_lbl_cookies_browser, m_cmb_cookies_browser);
        m_lbl_cookies_file = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_cookies);
        m_edt_cookies_path->setReadOnly(true);
        QHBoxLayout* cookies_file_row{ new QHBoxLayout() };
        cookies_file_row->addWidget(m_edt_cookies_path, 1);
        cookies_file_row->addWidget(m_btn_cookies_browse);
        cookies_file_row->addWidget(m_btn_cookies_clear);
        cookies_layout->addRow(m_lbl_cookies_file, cookies_file_row);
        layout->addStretch();
        translate();
        extractor::Options options{ configuration.extractor_options() };
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
        m_spn_speed_limit->setEnabled(m_chk_speed_limit->isChecked());
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
        m_lbl_speed_limit->setText(QString::fromStdString(translator._("{}:", translator._("Speed Limit (KiB/s)"))));
        m_grp_proxy->setTitle(translator._("Proxy"));
        m_lbl_proxy_url->setText(QString::fromStdString(translator._("{}:", translator._("URL"))));
        m_grp_cookies->setTitle(translator._("Cookies"));
        m_lbl_cookies_browser->setText(QString::fromStdString(translator._("{}:", translator._("Browser"))));
        QSignalBlocker cookies_browser_blocker{ m_cmb_cookies_browser };
        int cookies_browser_index{ m_cmb_cookies_browser->currentIndex() };
        m_cmb_cookies_browser->clear();
        m_cmb_cookies_browser->addItems({
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
        m_cmb_cookies_browser->setCurrentIndex(std::max(cookies_browser_index, 0));
        m_lbl_cookies_file->setText(QString::fromStdString(translator._("{}:", translator._("File"))));
        m_btn_cookies_browse->setText(translator._("Browse…"));
        m_btn_cookies_clear->setText(translator._("Clear"));
    }

    SettingsPostprocessingPage::SettingsPostprocessingPage(QWidget* parent)
        : QScrollArea{ parent },
          m_grp_metadata{ new oclero::qlementine::GroupBox(this) },
          m_chk_embed_metadata{ new QCheckBox(m_grp_metadata) },
          m_chk_remove_source_data{ new QCheckBox(m_grp_metadata) },
          m_grp_thumbnails{ new oclero::qlementine::GroupBox(this) },
          m_chk_embed_thumbnails{ new QCheckBox(m_grp_thumbnails) },
          m_chk_crop_audio_thumbnails{ new QCheckBox(m_grp_thumbnails) },
          m_grp_chapters{ new oclero::qlementine::GroupBox(this) },
          m_chk_embed_chapters{ new QCheckBox(m_grp_chapters) },
          m_grp_subtitles{ new oclero::qlementine::GroupBox(this) },
          m_chk_embed_subtitles{ new QCheckBox(m_grp_subtitles) },
          m_grp_ffmpeg{ new oclero::qlementine::GroupBox(this) },
          m_spn_threads{ new QSpinBox(m_grp_ffmpeg) },
          m_grp_arguments{ new oclero::qlementine::GroupBox(this) },
          m_btn_add{ new QToolButton(m_grp_arguments) },
          m_btn_edit{ new QToolButton(m_grp_arguments) },
          m_btn_remove{ new QToolButton(m_grp_arguments) },
          m_lst_arguments{ new QListWidget(m_grp_arguments) }
    {
        QWidget* content{ new QWidget() };
        QVBoxLayout* layout{ new QVBoxLayout(content) };
        layout->setContentsMargins(24, 20, 24, 20);
        layout->setSpacing(12);
        setFrameShape(QFrame::NoFrame);
        setWidgetResizable(true);
        setWidget(content);
        content->setBackgroundRole(QPalette::Base);
        configuration::Configuration& configuration{
            application::State::instance().configuration(),
        };
        QVBoxLayout* metadata_layout{ new QVBoxLayout(m_grp_metadata) };
        layout->addWidget(m_grp_metadata);
        metadata_layout->addWidget(m_chk_embed_metadata);
        metadata_layout->addWidget(m_chk_remove_source_data);
        m_dsc_remove_source_data =
                new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_metadata);
        m_dsc_remove_source_data->setWordWrap(true);
        m_dsc_remove_source_data->setContentsMargins(24, 0, 0, 0);
        metadata_layout->addWidget(m_dsc_remove_source_data);
        QVBoxLayout* thumbnails_layout{ new QVBoxLayout(m_grp_thumbnails) };
        layout->addWidget(m_grp_thumbnails);
        thumbnails_layout->addWidget(m_chk_embed_thumbnails);
        m_dsc_embed_thumbnails =
                new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_thumbnails);
        m_dsc_embed_thumbnails->setWordWrap(true);
        m_dsc_embed_thumbnails->setContentsMargins(24, 0, 0, 0);
        thumbnails_layout->addWidget(m_dsc_embed_thumbnails);
        thumbnails_layout->addWidget(m_chk_crop_audio_thumbnails);
        m_dsc_crop_audio_thumbnails =
                new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_thumbnails);
        m_dsc_crop_audio_thumbnails->setWordWrap(true);
        m_dsc_crop_audio_thumbnails->setContentsMargins(24, 0, 0, 0);
        thumbnails_layout->addWidget(m_dsc_crop_audio_thumbnails);
        QVBoxLayout* chapters_layout{ new QVBoxLayout(m_grp_chapters) };
        layout->addWidget(m_grp_chapters);
        chapters_layout->addWidget(m_chk_embed_chapters);
        QVBoxLayout* subtitles_layout{ new QVBoxLayout(m_grp_subtitles) };
        layout->addWidget(m_grp_subtitles);
        subtitles_layout->addWidget(m_chk_embed_subtitles);
        m_dsc_embed_subtitles =
                new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_subtitles);
        m_dsc_embed_subtitles->setWordWrap(true);
        m_dsc_embed_subtitles->setContentsMargins(24, 0, 0, 0);
        subtitles_layout->addWidget(m_dsc_embed_subtitles);
        QVBoxLayout* ffmpeg_layout{ new QVBoxLayout(m_grp_ffmpeg) };
        layout->addWidget(m_grp_ffmpeg);
        m_lbl_threads = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_ffmpeg);
        m_spn_threads->setRange(1, static_cast<int>(std::max(1U, std::thread::hardware_concurrency())));
        QHBoxLayout* threads_row{ new QHBoxLayout() };
        threads_row->addWidget(m_lbl_threads);
        threads_row->addWidget(m_spn_threads);
        threads_row->addStretch();
        ffmpeg_layout->addLayout(threads_row);
        m_dsc_threads = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_ffmpeg);
        m_dsc_threads->setWordWrap(true);
        ffmpeg_layout->addWidget(m_dsc_threads);
        QVBoxLayout* arguments_layout{ new QVBoxLayout(m_grp_arguments) };
        layout->addWidget(m_grp_arguments);
        m_btn_add->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(
                oclero::qlementine::icons::iconPath(oclero::qlementine::icons::Icons16::Action_Plus)));
        m_btn_edit->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(
                oclero::qlementine::icons::iconPath(oclero::qlementine::icons::Icons16::Misc_Pen)));
        m_btn_remove->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(
                oclero::qlementine::icons::iconPath(oclero::qlementine::icons::Icons16::Action_Minus)));
        QHBoxLayout* buttons{ new QHBoxLayout() };
        buttons->addWidget(m_btn_add);
        buttons->addWidget(m_btn_edit);
        buttons->addWidget(m_btn_remove);
        buttons->addStretch();
        arguments_layout->addLayout(buttons);
        m_lst_arguments->setMinimumHeight(120);
        arguments_layout->addWidget(m_lst_arguments);
        layout->addStretch();
        translate();
        postprocessing::Options options{ configuration.postprocessing_options() };
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
        connect(m_chk_embed_metadata, &QCheckBox::toggled, this, &SettingsPostprocessingPage::save);
        connect(m_chk_remove_source_data, &QCheckBox::toggled, this, &SettingsPostprocessingPage::save);
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
        postprocessing::Argument argument{ dialog.argument() };
        m_arguments.push_back(argument);
        m_lst_arguments->addItem(QString::fromStdString(argument.name()));
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
        postprocessing::Argument argument{ dialog.argument() };
        m_arguments[static_cast<std::size_t>(row)] = argument;
        m_lst_arguments->item(row)->setText(QString::fromStdString(argument.name()));
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
        m_chk_crop_audio_thumbnails->setEnabled(m_chk_embed_thumbnails->isChecked());
        m_chk_remove_source_data->setEnabled(m_chk_embed_metadata->isChecked());
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
        m_lbl_threads->setText(QString::fromStdString(translator._("{}:", translator._("Threads"))));
        m_dsc_threads->setText(translator._("Limit the number of threads used by ffmpeg."));
        m_grp_arguments->setTitle(translator._("Arguments"));
        m_grp_arguments->setDescription(
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
        setMinimumSize(700, 500);
        QVBoxLayout* layout{ new QVBoxLayout(this) };
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(m_navigation_view);
        m_navigation_view->sidebar()->setExpandedWidth(180);
        m_navigation_view->addPage(new SettingsGeneralPage(m_navigation_view), {},
                                   oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                           oclero::qlementine::icons::Icons16::Navigation_Settings)));
        m_navigation_view->addPage(new SettingsDiscoveryPage(m_navigation_view), {},
                                   oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                           oclero::qlementine::icons::Icons16::Navigation_Search)));
        m_navigation_view->addPage(new SettingsDownloadsPage(m_navigation_view), {},
                                   oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                           oclero::qlementine::icons::Icons16::Action_Download)));
        m_navigation_view->addPage(new SettingsExtractorPage(m_navigation_view), {},
                                   oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                           oclero::qlementine::icons::Icons16::File_Plugin)));
        m_navigation_view->addPage(new SettingsPostprocessingPage(m_navigation_view), {},
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
