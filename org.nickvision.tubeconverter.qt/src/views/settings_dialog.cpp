#include "views/settings_dialog.h"
#include <QApplication>
#include <QCheckBox>
#include <QCollator>
#include <QComboBox>
#include <QDesktopServices>
#include <QDialog>
#include <QFileDialog>
#include <QKeySequence>
#include <QLineEdit>
#include <QList>
#include <QListWidget>
#include <QMessageBox>
#include <QObject>
#include <QPalette>
#include <QPushButton>
#include <QShortcut>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QString>
#include <QStyleHints>
#include <QToolButton>
#include <QUrl>
#include <QWidget>
#include <Qt>
#include <QtCore/qtypes.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <oclero/qlementine/icons/Icons16.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/style/ThemeManager.hpp>
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
#include "controls/postprocessing_argument_dialog.h"
#include "cookies.h"
#include "discovery.h"
#include "download.h"
#include "extractor.h"
#include "helpers/translation_notifier.h"
#include "history.h"
#include "postprocessing.h"
#include "subtitle.h"
#include "translation.h"
#include "ui_settings_dialog.h"
#include "update.h"
#include "video.h"

namespace parabolic::qt::views
{
    SettingsDialog::SettingsDialog(QWidget* parent)
        : QDialog{ parent },
          m_languages{ application::State::instance().translator().available_languages() },
          m_ui{ new Ui::SettingsDialog() }
    {
        m_ui->setupUi(this);
        m_ui->navigation_view->sidebar()->setExpandedWidth(180);
        m_ui->navigation_view->addPage(m_ui->page_general,
                                       {},
                                       oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                               oclero::qlementine::icons::Icons16::Navigation_Settings)));
        m_ui->navigation_view->addPage(m_ui->page_discovery,
                                       {},
                                       oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                               oclero::qlementine::icons::Icons16::Navigation_Search)));
        m_ui->navigation_view->addPage(m_ui->page_downloads,
                                       {},
                                       oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                               oclero::qlementine::icons::Icons16::Action_Download)));
        m_ui->navigation_view->addPage(m_ui->page_extractor,
                                       {},
                                       oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                               oclero::qlementine::icons::Icons16::File_Plugin)));
        m_ui->navigation_view->addPage(m_ui->page_postprocessing,
                                       {},
                                       oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                               oclero::qlementine::icons::Icons16::Action_Build)));
        m_ui->general_content->setBackgroundRole(QPalette::Base);
        m_ui->discovery_content->setBackgroundRole(QPalette::Base);
        m_ui->downloads_content->setBackgroundRole(QPalette::Base);
        m_ui->extractor_content->setBackgroundRole(QPalette::Base);
        m_ui->postprocessing_content->setBackgroundRole(QPalette::Base);
        configuration::Configuration& configuration{ application::State::instance().configuration() };
        m_ui->cmb_language->addItems(QList<QString>(static_cast<qsizetype>(m_languages.size()) + 1));
        m_ui->btn_learn_more->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(
                oclero::qlementine::icons::iconPath(oclero::qlementine::icons::Icons16::Misc_Info)));
        m_ui->btn_cookies_browse->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(
                oclero::qlementine::icons::iconPath(oclero::qlementine::icons::Icons16::File_Folder)));
        m_ui->btn_cookies_clear->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(
                oclero::qlementine::icons::iconPath(oclero::qlementine::icons::Icons16::Action_Clear)));
        m_ui->spn_threads->setRange(1, static_cast<int>(std::max(1U, std::thread::hardware_concurrency())));
        m_ui->btn_add->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(
                oclero::qlementine::icons::iconPath(oclero::qlementine::icons::Icons16::Action_Plus)));
        m_ui->btn_edit->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(
                oclero::qlementine::icons::iconPath(oclero::qlementine::icons::Icons16::Misc_Pen)));
        m_ui->btn_remove->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(
                oclero::qlementine::icons::iconPath(oclero::qlementine::icons::Icons16::Action_Minus)));
        translate();
        std::vector<std::string>::iterator language_it{ std::ranges::find(m_languages,
                                                                          configuration.translation_options().language()) };
        m_ui->cmb_theme->setCurrentIndex(static_cast<int>(configuration.theme()));
        m_ui->cmb_language->setCurrentIndex(
                language_it == m_languages.end() ? 0 : static_cast<int>(language_it - m_languages.begin()) + 1);
        m_ui->chk_automatic_updates->setChecked(configuration.automatically_check_for_updates());
        m_ui->chk_preview_updates->setChecked(configuration.update_options().allow_preview_updates());
        switch (configuration.history_options().length())
        {
        case history::Length::Never:
            m_ui->cmb_history_length->setCurrentIndex(0);
            break;
        case history::Length::OneDay:
            m_ui->cmb_history_length->setCurrentIndex(1);
            break;
        case history::Length::OneWeek:
            m_ui->cmb_history_length->setCurrentIndex(2);
            break;
        case history::Length::OneMonth:
            m_ui->cmb_history_length->setCurrentIndex(3);
            break;
        case history::Length::ThreeMonths:
            m_ui->cmb_history_length->setCurrentIndex(4);
            break;
        case history::Length::SixMonths:
            m_ui->cmb_history_length->setCurrentIndex(5);
            break;
        case history::Length::OneYear:
            m_ui->cmb_history_length->setCurrentIndex(6);
            break;
        case history::Length::Forever:
            m_ui->cmb_history_length->setCurrentIndex(7);
            break;
        }
        connect(m_ui->cmb_theme, &QComboBox::currentIndexChanged, this, &SettingsDialog::change_theme);
        connect(m_ui->cmb_language, &QComboBox::currentIndexChanged, this, &SettingsDialog::change_language);
        connect(m_ui->chk_automatic_updates, &QCheckBox::toggled, this, &SettingsDialog::save_general);
        connect(m_ui->chk_preview_updates, &QCheckBox::toggled, this, &SettingsDialog::save_general);
        connect(m_ui->cmb_history_length, &QComboBox::currentIndexChanged, this, &SettingsDialog::save_general);
        discovery::Options discovery_options{ configuration.discovery_options() };
        m_ui->chk_media_id->setChecked(discovery_options.include_media_id_in_title());
        m_ui->chk_limit_characters->setChecked(discovery_options.limit_characters());
        m_ui->chk_super_resolutions->setChecked(discovery_options.include_super_resolutions());
        m_ui->chk_auto_generated_subtitles->setChecked(discovery_options.include_auto_generated_subtitles());
        m_ui->cmb_audio_codec->setCurrentIndex(static_cast<int>(discovery_options.preferred_audio_codec()));
        m_ui->cmb_video_codec->setCurrentIndex(static_cast<int>(discovery_options.preferred_video_codec()));
        m_ui->cmb_subtitle_format->setCurrentIndex(static_cast<int>(discovery_options.preferred_subtitle_format()));
        m_ui->cmb_frame_rate->setCurrentIndex(static_cast<int>(discovery_options.preferred_frame_rate()));
        m_ui->txt_discovery_arguments->setText(QString::fromStdString(discovery_options.ytdlp_arguments()));
        connect(m_ui->chk_media_id, &QCheckBox::toggled, this, &SettingsDialog::save_discovery);
        connect(m_ui->chk_limit_characters, &QCheckBox::toggled, this, &SettingsDialog::save_discovery);
        connect(m_ui->chk_super_resolutions, &QCheckBox::toggled, this, &SettingsDialog::save_discovery);
        connect(m_ui->chk_auto_generated_subtitles, &QCheckBox::toggled, this, &SettingsDialog::save_discovery);
        connect(m_ui->cmb_audio_codec, &QComboBox::currentIndexChanged, this, &SettingsDialog::save_discovery);
        connect(m_ui->cmb_video_codec, &QComboBox::currentIndexChanged, this, &SettingsDialog::save_discovery);
        connect(m_ui->cmb_subtitle_format, &QComboBox::currentIndexChanged, this, &SettingsDialog::save_discovery);
        connect(m_ui->cmb_frame_rate, &QComboBox::currentIndexChanged, this, &SettingsDialog::save_discovery);
        connect(m_ui->txt_discovery_arguments, &QLineEdit::textChanged, this, &SettingsDialog::save_discovery);
        download::Options download_options{ configuration.download_options() };
        aria2::Options aria2_options{ configuration.aria2_options() };
        m_ui->spn_max_downloads->setValue(download_options.max_number_of_active_downloads());
        m_ui->chk_overwrite_existing_files->setChecked(download_options.overwrite_existing_files());
        m_ui->chk_prevent_suspend->setChecked(download_options.prevent_suspend());
        m_ui->chk_use_aria->setChecked(aria2_options.use_aria());
        m_ui->spn_aria_connections->setValue(aria2_options.max_connections_per_server());
        m_ui->spn_aria_connections->setEnabled(aria2_options.use_aria());
        m_ui->spn_aria_split_size->setValue(aria2_options.min_split_size());
        m_ui->spn_aria_split_size->setEnabled(aria2_options.use_aria());
        m_ui->txt_downloads_arguments->setText(QString::fromStdString(download_options.ytdlp_arguments()));
        connect(m_ui->spn_max_downloads, &QSpinBox::valueChanged, this, &SettingsDialog::save_downloads);
        connect(m_ui->chk_overwrite_existing_files, &QCheckBox::toggled, this, &SettingsDialog::save_downloads);
        connect(m_ui->chk_prevent_suspend, &QCheckBox::toggled, this, &SettingsDialog::save_downloads);
        connect(m_ui->chk_use_aria, &QCheckBox::toggled, this, &SettingsDialog::save_downloads);
        connect(m_ui->btn_learn_more, &QToolButton::clicked, this, &SettingsDialog::learn_more);
        connect(m_ui->spn_aria_connections, &QSpinBox::valueChanged, this, &SettingsDialog::save_downloads);
        connect(m_ui->spn_aria_split_size, &QSpinBox::valueChanged, this, &SettingsDialog::save_downloads);
        connect(m_ui->txt_downloads_arguments, &QLineEdit::textChanged, this, &SettingsDialog::save_downloads);
        extractor::Options extractor_options{ configuration.extractor_options() };
        m_ui->chk_part_files->setChecked(extractor_options.use_part_files());
        m_ui->chk_sponsor_block->setChecked(extractor_options.youtube_sponsor_block());
        m_ui->chk_translate_metadata->setChecked(extractor_options.translate_metadata_and_chapters());
        m_ui->chk_speed_limit->setChecked(extractor_options.speed_limit().has_value());
        m_ui->spn_speed_limit->setValue(extractor_options.speed_limit().value_or(1024));
        m_ui->spn_speed_limit->setEnabled(extractor_options.speed_limit().has_value());
        m_ui->txt_proxy_url->setText(QString::fromStdString(extractor_options.proxy_url()));
        m_ui->cmb_cookies_browser->setCurrentIndex(static_cast<int>(extractor_options.cookies_browser()));
        m_ui->txt_cookies_path->setText(QString::fromStdString(extractor_options.cookies_path()));
        connect(m_ui->chk_part_files, &QCheckBox::toggled, this, &SettingsDialog::save_extractor);
        connect(m_ui->chk_sponsor_block, &QCheckBox::toggled, this, &SettingsDialog::save_extractor);
        connect(m_ui->chk_translate_metadata, &QCheckBox::toggled, this, &SettingsDialog::save_extractor);
        connect(m_ui->chk_speed_limit, &QCheckBox::toggled, this, &SettingsDialog::save_extractor);
        connect(m_ui->spn_speed_limit, &QSpinBox::valueChanged, this, &SettingsDialog::save_extractor);
        connect(m_ui->txt_proxy_url, &QLineEdit::textChanged, this, &SettingsDialog::save_extractor);
        connect(m_ui->cmb_cookies_browser, &QComboBox::currentIndexChanged, this, &SettingsDialog::save_extractor);
        connect(m_ui->txt_cookies_path, &QLineEdit::textChanged, this, &SettingsDialog::save_extractor);
        connect(m_ui->btn_cookies_browse, &QPushButton::clicked, this, &SettingsDialog::browse_cookies_file);
        connect(m_ui->btn_cookies_clear, &QPushButton::clicked, m_ui->txt_cookies_path, &QLineEdit::clear);
        postprocessing::Options postprocessing_options{ configuration.postprocessing_options() };
        m_ui->chk_embed_metadata->setChecked(postprocessing_options.embed_metadata());
        m_ui->chk_remove_source_data->setChecked(postprocessing_options.remove_source_data());
        m_ui->chk_remove_source_data->setEnabled(postprocessing_options.embed_metadata());
        m_ui->chk_embed_thumbnails->setChecked(postprocessing_options.embed_thumbnails());
        m_ui->chk_crop_audio_thumbnails->setChecked(postprocessing_options.crop_audio_thumbnails());
        m_ui->chk_crop_audio_thumbnails->setEnabled(postprocessing_options.embed_thumbnails());
        m_ui->chk_embed_chapters->setChecked(postprocessing_options.embed_chapters());
        m_ui->chk_embed_subtitles->setChecked(postprocessing_options.embed_subtitles());
        m_ui->spn_threads->setValue(postprocessing_options.threads());
        m_arguments = postprocessing_options.arguments();
        for (const postprocessing::Argument& argument : m_arguments)
        {
            m_ui->lst_arguments->addItem(QString::fromStdString(argument.name()));
        }
        connect(m_ui->chk_embed_metadata, &QCheckBox::toggled, this, &SettingsDialog::save_postprocessing);
        connect(m_ui->chk_remove_source_data, &QCheckBox::toggled, this, &SettingsDialog::save_postprocessing);
        connect(m_ui->chk_embed_thumbnails, &QCheckBox::toggled, this, &SettingsDialog::save_postprocessing);
        connect(m_ui->chk_crop_audio_thumbnails, &QCheckBox::toggled, this, &SettingsDialog::save_postprocessing);
        connect(m_ui->chk_embed_chapters, &QCheckBox::toggled, this, &SettingsDialog::save_postprocessing);
        connect(m_ui->chk_embed_subtitles, &QCheckBox::toggled, this, &SettingsDialog::save_postprocessing);
        connect(m_ui->spn_threads, &QSpinBox::valueChanged, this, &SettingsDialog::save_postprocessing);
        connect(m_ui->btn_add, &QToolButton::clicked, this, &SettingsDialog::add_argument);
        connect(m_ui->btn_edit, &QToolButton::clicked, this, &SettingsDialog::edit_argument);
        connect(m_ui->btn_remove, &QToolButton::clicked, this, &SettingsDialog::remove_argument);
        connect(m_ui->lst_arguments, &QListWidget::itemDoubleClicked, this, &SettingsDialog::edit_argument);
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed,
                this,
                &SettingsDialog::translate);
        connect(new QShortcut(QKeySequence::Close, this), &QShortcut::activated, this, &QDialog::reject);
    }

    SettingsDialog::~SettingsDialog()
    {
        delete m_ui;
    }

    void SettingsDialog::add_argument()
    {
        std::vector<std::string> names;
        names.reserve(m_arguments.size());
        for (const postprocessing::Argument& argument : m_arguments)
        {
            names.push_back(argument.name());
        }
        controls::PostprocessingArgumentDialog dialog{ { "", postprocessing::Step::None, postprocessing::Executable::None, "" },
                                                       names,
                                                       this };
        if (dialog.exec() != QDialog::Accepted)
        {
            return;
        }
        postprocessing::Argument argument{ dialog.argument() };
        m_arguments.push_back(argument);
        m_ui->lst_arguments->addItem(QString::fromStdString(argument.name()));
        m_ui->lst_arguments->setCurrentRow(m_ui->lst_arguments->count() - 1);
        save_postprocessing();
    }

    void SettingsDialog::browse_cookies_file()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        QString path{
            QFileDialog::getOpenFileName(this, translator._("Cookies File"), {}, translator._("Text Files (*.txt)"))
        };
        if (!path.isEmpty())
        {
            m_ui->txt_cookies_path->setText(path);
        }
    }

    void SettingsDialog::change_language()
    {
        configuration::Configuration& configuration{ application::State::instance().configuration() };
        translation::Options options{ configuration.translation_options() };
        options.set_language(m_ui->cmb_language->currentIndex() == 0
                                     ? translation::Translator::follow_system_language()
                                     : m_languages[static_cast<std::size_t>(m_ui->cmb_language->currentIndex()) - 1]);
        configuration.set_translation_options(options);
        qobject_cast<App*>(QApplication::instance())->translation_notifier()->translation_changed();
    }

    void SettingsDialog::change_theme()
    {
        oclero::qlementine::ThemeManager* theme_manager{ qobject_cast<App*>(QApplication::instance())->theme_manager() };
        configuration::Theme theme{ static_cast<configuration::Theme>(m_ui->cmb_theme->currentIndex()) };
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

    void SettingsDialog::edit_argument()
    {
        int row{ m_ui->lst_arguments->currentRow() };
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
        controls::PostprocessingArgumentDialog dialog{ m_arguments[static_cast<std::size_t>(row)], names, this };
        if (dialog.exec() != QDialog::Accepted)
        {
            return;
        }
        postprocessing::Argument argument{ dialog.argument() };
        m_arguments[static_cast<std::size_t>(row)] = argument;
        m_ui->lst_arguments->item(row)->setText(QString::fromStdString(argument.name()));
        save_postprocessing();
    }

    void SettingsDialog::learn_more()
    {
        QDesktopServices::openUrl(QUrl("https://aria2.github.io"));
    }

    void SettingsDialog::remove_argument()
    {
        int row{ m_ui->lst_arguments->currentRow() };
        if (row < 0)
        {
            return;
        }
        const translation::Translator& translator{ application::State::instance().translator() };
        if (QMessageBox::question(this,
                                  translator._("Remove Argument"),
                                  QString::fromStdString(translator._("Are you sure you want to remove \"{}\"?",
                                                                      m_arguments[static_cast<std::size_t>(row)].name()))) !=
            QMessageBox::Yes)
        {
            return;
        }
        m_arguments.erase(m_arguments.begin() + row);
        delete m_ui->lst_arguments->takeItem(row);
        save_postprocessing();
    }

    void SettingsDialog::save_discovery()
    {
        configuration::Configuration& configuration{ application::State::instance().configuration() };
        discovery::Options options{ configuration.discovery_options() };
        options.set_include_media_id_in_title(m_ui->chk_media_id->isChecked());
        options.set_limit_characters(m_ui->chk_limit_characters->isChecked());
        options.set_include_super_resolutions(m_ui->chk_super_resolutions->isChecked());
        options.set_include_auto_generated_subtitles(m_ui->chk_auto_generated_subtitles->isChecked());
        options.set_preferred_audio_codec(static_cast<audio::Codec>(m_ui->cmb_audio_codec->currentIndex()));
        options.set_preferred_video_codec(static_cast<video::Codec>(m_ui->cmb_video_codec->currentIndex()));
        options.set_preferred_subtitle_format(static_cast<subtitle::Type>(m_ui->cmb_subtitle_format->currentIndex()));
        options.set_preferred_frame_rate(static_cast<video::FrameRate>(m_ui->cmb_frame_rate->currentIndex()));
        options.set_ytdlp_arguments(m_ui->txt_discovery_arguments->text().toStdString());
        configuration.set_discovery_options(options);
    }

    void SettingsDialog::save_downloads()
    {
        m_ui->spn_aria_connections->setEnabled(m_ui->chk_use_aria->isChecked());
        m_ui->spn_aria_split_size->setEnabled(m_ui->chk_use_aria->isChecked());
        configuration::Configuration& configuration{ application::State::instance().configuration() };
        download::Options download_options{ configuration.download_options() };
        download_options.set_max_number_of_active_downloads(m_ui->spn_max_downloads->value());
        download_options.set_overwrite_existing_files(m_ui->chk_overwrite_existing_files->isChecked());
        download_options.set_prevent_suspend(m_ui->chk_prevent_suspend->isChecked());
        download_options.set_ytdlp_arguments(m_ui->txt_downloads_arguments->text().toStdString());
        configuration.set_download_options(download_options);
        aria2::Options aria2_options{ configuration.aria2_options() };
        aria2_options.set_use_aria(m_ui->chk_use_aria->isChecked());
        aria2_options.set_max_connections_per_server(m_ui->spn_aria_connections->value());
        aria2_options.set_min_split_size(m_ui->spn_aria_split_size->value());
        configuration.set_aria2_options(aria2_options);
    }

    void SettingsDialog::save_extractor()
    {
        m_ui->spn_speed_limit->setEnabled(m_ui->chk_speed_limit->isChecked());
        configuration::Configuration& configuration{ application::State::instance().configuration() };
        extractor::Options options{ configuration.extractor_options() };
        options.set_use_part_files(m_ui->chk_part_files->isChecked());
        options.set_youtube_sponsor_block(m_ui->chk_sponsor_block->isChecked());
        options.set_translate_metadata_and_chapters(m_ui->chk_translate_metadata->isChecked());
        options.set_speed_limit(m_ui->chk_speed_limit->isChecked() ? std::optional<int>{ m_ui->spn_speed_limit->value() }
                                                                   : std::nullopt);
        options.set_proxy_url(m_ui->txt_proxy_url->text().toStdString());
        options.set_cookies_browser(static_cast<cookies::Browser>(m_ui->cmb_cookies_browser->currentIndex()));
        options.set_cookies_path(m_ui->txt_cookies_path->text().toStdString());
        configuration.set_extractor_options(options);
    }

    void SettingsDialog::save_general()
    {
        configuration::Configuration& configuration{ application::State::instance().configuration() };
        configuration.set_automatically_check_for_updates(m_ui->chk_automatic_updates->isChecked());
        update::Options update_options{ configuration.update_options() };
        update_options.set_allow_preview_updates(m_ui->chk_preview_updates->isChecked());
        configuration.set_update_options(update_options);
        history::Options history_options{ configuration.history_options() };
        switch (m_ui->cmb_history_length->currentIndex())
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

    void SettingsDialog::save_postprocessing()
    {
        m_ui->chk_crop_audio_thumbnails->setEnabled(m_ui->chk_embed_thumbnails->isChecked());
        m_ui->chk_remove_source_data->setEnabled(m_ui->chk_embed_metadata->isChecked());
        configuration::Configuration& configuration{ application::State::instance().configuration() };
        postprocessing::Options options{ configuration.postprocessing_options() };
        options.set_embed_metadata(m_ui->chk_embed_metadata->isChecked());
        options.set_remove_source_data(m_ui->chk_remove_source_data->isChecked());
        options.set_embed_thumbnails(m_ui->chk_embed_thumbnails->isChecked());
        options.set_crop_audio_thumbnails(m_ui->chk_crop_audio_thumbnails->isChecked());
        options.set_embed_chapters(m_ui->chk_embed_chapters->isChecked());
        options.set_embed_subtitles(m_ui->chk_embed_subtitles->isChecked());
        options.set_threads(m_ui->spn_threads->value());
        options.set_arguments(m_arguments);
        configuration.set_postprocessing_options(options);
    }

    void SettingsDialog::translate()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        QSignalBlocker theme_blocker{ m_ui->cmb_theme };
        QSignalBlocker language_blocker{ m_ui->cmb_language };
        QSignalBlocker history_length_blocker{ m_ui->cmb_history_length };
        QSignalBlocker audio_codec_blocker{ m_ui->cmb_audio_codec };
        QSignalBlocker video_codec_blocker{ m_ui->cmb_video_codec };
        QSignalBlocker subtitle_format_blocker{ m_ui->cmb_subtitle_format };
        QSignalBlocker frame_rate_blocker{ m_ui->cmb_frame_rate };
        QSignalBlocker cookies_browser_blocker{ m_ui->cmb_cookies_browser };
        int theme_index{ m_ui->cmb_theme->currentIndex() };
        int history_length_index{ m_ui->cmb_history_length->currentIndex() };
        int audio_codec_index{ m_ui->cmb_audio_codec->currentIndex() };
        int video_codec_index{ m_ui->cmb_video_codec->currentIndex() };
        int subtitle_format_index{ m_ui->cmb_subtitle_format->currentIndex() };
        int frame_rate_index{ m_ui->cmb_frame_rate->currentIndex() };
        int cookies_browser_index{ m_ui->cmb_cookies_browser->currentIndex() };
        std::string selected_language{ m_ui->cmb_language->currentIndex() > 0
                                               ? m_languages[static_cast<std::size_t>(m_ui->cmb_language->currentIndex()) - 1]
                                               : std::string{} };
        setWindowTitle(translator._("Settings"));
        m_ui->navigation_view->sidebar()->setItemText(0, translator._p("Settings", "General"));
        m_ui->navigation_view->sidebar()->setItemText(1, translator._p("Settings", "Discovery"));
        m_ui->navigation_view->sidebar()->setItemText(2, translator._p("Settings", "Downloads"));
        m_ui->navigation_view->sidebar()->setItemText(3, translator._p("Settings", "Extractor"));
        m_ui->navigation_view->sidebar()->setItemText(4, translator._p("Settings", "Postprocessing"));
        m_ui->grp_user_interface->setTitle(translator._("User Interface"));
        m_ui->lbl_theme->setText(QString::fromStdString(translator._("{}:", translator._("Theme"))));
        m_ui->cmb_theme->clear();
        m_ui->cmb_theme->addItems(
                { translator._p("Theme", "Light"), translator._p("Theme", "Dark"), translator._p("Theme", "System") });
        m_ui->cmb_theme->setCurrentIndex(std::max(theme_index, 0));
        m_ui->lbl_language->setText(QString::fromStdString(translator._("{}:", translator._("Translation Language"))));
        m_ui->cmb_language->setItemText(0, translator._p("Language", "System"));
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
            m_ui->cmb_language->setItemText(static_cast<int>(i) + 1, languages[i].first);
        }
        std::vector<std::string>::iterator selected_it{ std::ranges::find(m_languages, selected_language) };
        m_ui->cmb_language->setCurrentIndex(selected_it == m_languages.end() || selected_language.empty()
                                                    ? 0
                                                    : static_cast<int>(selected_it - m_languages.begin()) + 1);
        m_ui->grp_updates->setTitle(translator._("Updates"));
        m_ui->chk_automatic_updates->setText(translator._("Automatically Check for Updates"));
        m_ui->chk_preview_updates->setText(translator._("Allow Preview Updates"));
        m_ui->lbl_preview_updates->setText(translator._("Update the app and dependencies, such as yt-dlp, to beta versions."));
        m_ui->grp_history->setTitle(translator._("History"));
        m_ui->lbl_history_length->setText(
                QString::fromStdString(translator._("{}:", translator._("Keep Download History For"))));
        m_ui->cmb_history_length->clear();
        m_ui->cmb_history_length->addItems({ translator._("Never"),
                                             translator._("1 Day"),
                                             translator._("1 Week"),
                                             translator._("1 Month"),
                                             translator._("3 Months"),
                                             translator._("6 Months"),
                                             translator._("1 Year"),
                                             translator._("Forever") });
        m_ui->cmb_history_length->setCurrentIndex(std::max(history_length_index, 0));
        m_ui->grp_discovery_process->setTitle(translator._("Process"));
        m_ui->chk_media_id->setText(translator._("Include Media Id in Title"));
        m_ui->lbl_media_id->setText(translator._("Add the media entry's yt-dlp id to its default title."));
        m_ui->chk_limit_characters->setText(translator._("Limit Filename Characters"));
        m_ui->lbl_limit_characters->setText(translator._("Restrict filename characters to those supported only by Windows."));
        m_ui->chk_super_resolutions->setText(translator._("Show Super Resolution Formats"));
        m_ui->lbl_super_resolutions->setText(
                translator._("Super resolutions are resolutions that have been enhanced with artificial intelligence."));
        m_ui->chk_auto_generated_subtitles->setText(translator._("Show Auto-Generated Subtitles"));
        m_ui->grp_preferences->setTitle(translator._("Preferences"));
        m_ui->grp_preferences->setDescription(
                translator._("Prefer these codecs and options when showing formats for download."));
        m_ui->lbl_audio_codec->setText(QString::fromStdString(translator._("{}:", translator._("Audio Codec"))));
        m_ui->cmb_audio_codec->clear();
        m_ui->cmb_audio_codec->addItems({ translator._("Any"),
                                          translator._("FLAC"),
                                          translator._("WAV"),
                                          translator._("Opus"),
                                          translator._("AAC"),
                                          translator._("MP4A"),
                                          translator._("MP3") });
        m_ui->cmb_audio_codec->setCurrentIndex(std::max(audio_codec_index, 0));
        m_ui->lbl_video_codec->setText(QString::fromStdString(translator._("{}:", translator._("Video Codec"))));
        m_ui->cmb_video_codec->clear();
        m_ui->cmb_video_codec->addItems({ translator._("Any"),
                                          translator._("H.265"),
                                          translator._("VP9"),
                                          translator._("AV1"),
                                          translator._("H.264") });
        m_ui->cmb_video_codec->setCurrentIndex(std::max(video_codec_index, 0));
        m_ui->lbl_subtitle_format->setText(QString::fromStdString(translator._("{}:", translator._("Subtitle Format"))));
        m_ui->cmb_subtitle_format->clear();
        m_ui->cmb_subtitle_format->addItems(
                { translator._("Any"), translator._("VTT"), translator._("SRT"), translator._("ASS"), translator._("LRC") });
        m_ui->cmb_subtitle_format->setCurrentIndex(std::max(subtitle_format_index, 0));
        m_ui->lbl_frame_rate->setText(QString::fromStdString(translator._("{}:", translator._("Frame Rate"))));
        m_ui->cmb_frame_rate->clear();
        m_ui->cmb_frame_rate->addItems(
                { translator._("Any"), translator._("24 FPS"), translator._("30 FPS"), translator._("60 FPS") });
        m_ui->cmb_frame_rate->setCurrentIndex(std::max(frame_rate_index, 0));
        m_ui->grp_discovery_advanced->setTitle(translator._("Advanced"));
        m_ui->lbl_discovery_arguments->setText(QString::fromStdString(translator._("{}:", translator._("Arguments"))));
        m_ui->lbl_discovery_arguments_description->setText(
                translator._("These arguments will be appended to yt-dlp during the discovery phase."));
        m_ui->grp_downloads_process->setTitle(translator._("Process"));
        m_ui->lbl_max_downloads->setText(QString::fromStdString(translator._("{}:", translator._("Maximum Active Downloads"))));
        m_ui->chk_overwrite_existing_files->setText(translator._("Overwrite Existing Files"));
        m_ui->chk_prevent_suspend->setText(translator._("Prevent Sleep During Downloads"));
        m_ui->grp_aria->setTitle(translator._("aria2"));
        m_ui->grp_aria->setDescription(translator._("An alternative downloader that may be faster in some regions compared to "
                                                    "yt-dlp's native downloader."));
        m_ui->chk_use_aria->setText(translator._("Use aria2"));
        m_ui->btn_learn_more->setToolTip(translator._("Learn More"));
        m_ui->lbl_aria_connections->setText(
                QString::fromStdString(translator._("{}:", translator._("Maximum Connections Per Server"))));
        m_ui->lbl_aria_split_size->setText(
                QString::fromStdString(translator._("{}:", translator._("Minimum Split Size (MiB)"))));
        m_ui->grp_downloads_advanced->setTitle(translator._("Advanced"));
        m_ui->lbl_downloads_arguments->setText(QString::fromStdString(translator._("{}:", translator._("Arguments"))));
        m_ui->lbl_downloads_arguments_description->setText(
                translator._("These arguments will be appended to yt-dlp during the download phase."));
        m_ui->grp_extractor_process->setTitle(translator._("Process"));
        m_ui->chk_part_files->setText(translator._("Use Part Files"));
        m_ui->lbl_part_files->setText(
                translator._("Download media in separate .part files instead of directly into the output file."));
        m_ui->chk_sponsor_block->setText(translator._("Enable SponsorBlock for YouTube"));
        m_ui->lbl_sponsor_block->setText(translator._("Try to remove sponsored segments from videos."));
        m_ui->chk_translate_metadata->setText(translator._("Translate Metadata and Chapters"));
        m_ui->lbl_translate_metadata->setText(translator._("Automatically translate embedded metadata and chapters using "
                                                           "the app's set language on supported sites and media."));
        m_ui->grp_speed->setTitle(translator._("Speed"));
        m_ui->chk_speed_limit->setText(translator._("Limit Download Speed"));
        m_ui->lbl_speed_limit->setText(QString::fromStdString(translator._("{}:", translator._("Speed Limit (KiB/s)"))));
        m_ui->grp_proxy->setTitle(translator._("Proxy"));
        m_ui->lbl_proxy_url->setText(QString::fromStdString(translator._("{}:", translator._("URL"))));
        m_ui->grp_cookies->setTitle(translator._("Cookies"));
        m_ui->lbl_cookies_browser->setText(QString::fromStdString(translator._("{}:", translator._("Browser"))));
        m_ui->cmb_cookies_browser->clear();
        m_ui->cmb_cookies_browser->addItems({ translator._("None"),
                                              translator._("Brave"),
                                              translator._("Chrome"),
                                              translator._("Chromium"),
                                              translator._("Edge"),
                                              translator._("Firefox"),
                                              translator._("Opera"),
                                              translator._("Vivaldi"),
                                              translator._("Whale") });
        m_ui->cmb_cookies_browser->setCurrentIndex(std::max(cookies_browser_index, 0));
        m_ui->lbl_cookies_browser_description->setText(
                translator._("Selecting a browser other than \"None\" will use it instead of the cookies file."));
        m_ui->lbl_cookies_file->setText(QString::fromStdString(translator._("{}:", translator._("File"))));
        m_ui->btn_cookies_browse->setToolTip(translator._("Browse"));
        m_ui->btn_cookies_clear->setToolTip(translator._("Clear"));
        m_ui->grp_metadata->setTitle(translator._("Metadata"));
        m_ui->chk_embed_metadata->setText(translator._("Embed Metadata"));
        m_ui->chk_remove_source_data->setText(translator._("Remove Source Data"));
        m_ui->lbl_remove_source_data->setText(
                translator._("Clear metadata fields containing identifying download information."));
        m_ui->grp_thumbnails->setTitle(translator._("Thumbnails"));
        m_ui->chk_embed_thumbnails->setText(translator._("Embed Thumbnails"));
        m_ui->lbl_embed_thumbnails->setText(translator._("If the file type does not support embedding, the thumbnail will "
                                                         "be written to a separate image file."));
        m_ui->chk_crop_audio_thumbnails->setText(translator._("Crop Audio Thumbnails"));
        m_ui->lbl_crop_audio_thumbnails->setText(translator._("Crop thumbnails of audio files to squares."));
        m_ui->grp_chapters->setTitle(translator._("Chapters"));
        m_ui->chk_embed_chapters->setText(translator._("Embed Chapters"));
        m_ui->grp_subtitles->setTitle(translator._("Subtitles"));
        m_ui->chk_embed_subtitles->setText(translator._("Embed Subtitles"));
        m_ui->lbl_embed_subtitles->setText(translator._(
                "If disabled or if embedding is not supported, downloaded subtitles will be saved to separate files."));
        m_ui->grp_ffmpeg->setTitle(translator._("FFmpeg"));
        m_ui->lbl_threads->setText(QString::fromStdString(translator._("{}:", translator._("Threads"))));
        m_ui->lbl_threads_description->setText(translator._("Limit the number of threads used by ffmpeg."));
        m_ui->grp_arguments->setTitle(translator._("Arguments"));
        m_ui->grp_arguments->setDescription(
                translator._("Arguments will be shown for selection in the download discovery dialog."));
        m_ui->btn_add->setToolTip(translator._("Add"));
        m_ui->btn_edit->setToolTip(translator._("Edit"));
        m_ui->btn_remove->setToolTip(translator._("Remove"));
    }
}
