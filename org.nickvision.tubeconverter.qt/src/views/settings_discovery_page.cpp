#include "views/settings_discovery_page.h"
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QFrame>
#include <QLineEdit>
#include <QObject>
#include <QPalette>
#include <QScrollArea>
#include <QSignalBlocker>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>
#include <algorithm>
#include <oclero/qlementine/Common.hpp>
#include <oclero/qlementine/widgets/GroupBox.hpp>
#include <oclero/qlementine/widgets/Label.hpp>
#include "app.h"
#include "application.h"
#include "audio.h"
#include "configuration.h"
#include "discovery.h"
#include "helpers/translation_notifier.h"
#include "subtitle.h"
#include "translation.h"
#include "video.h"

namespace parabolic::qt::views
{
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
          m_grp_advanced{ new oclero::qlementine::GroupBox(this) },
          m_txt_arguments{ new QLineEdit(m_grp_advanced) }
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
        QVBoxLayout* process_layout{ new QVBoxLayout(m_grp_process) };
        layout->addWidget(m_grp_process);
        process_layout->addWidget(m_chk_media_id);
        m_lbl_media_id = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_process);
        m_lbl_media_id->setWordWrap(true);
        m_lbl_media_id->setContentsMargins(24, 0, 0, 0);
        process_layout->addWidget(m_lbl_media_id);
        process_layout->addWidget(m_chk_limit_characters);
        m_lbl_limit_characters = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_process);
        m_lbl_limit_characters->setWordWrap(true);
        m_lbl_limit_characters->setContentsMargins(24, 0, 0, 0);
        process_layout->addWidget(m_lbl_limit_characters);
        process_layout->addWidget(m_chk_super_resolutions);
        m_lbl_super_resolutions = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_process);
        m_lbl_super_resolutions->setWordWrap(true);
        m_lbl_super_resolutions->setContentsMargins(24, 0, 0, 0);
        process_layout->addWidget(m_lbl_super_resolutions);
        process_layout->addWidget(m_chk_auto_generated_subtitles);
        QFormLayout* preferences_layout{ new QFormLayout(m_grp_preferences) };
        preferences_layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
        layout->addWidget(m_grp_preferences);
        m_lbl_audio_codec = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_preferences);
        preferences_layout->addRow(m_lbl_audio_codec, m_cmb_audio_codec);
        m_lbl_video_codec = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_preferences);
        preferences_layout->addRow(m_lbl_video_codec, m_cmb_video_codec);
        m_lbl_subtitle_format = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_preferences);
        preferences_layout->addRow(m_lbl_subtitle_format, m_cmb_subtitle_format);
        m_lbl_frame_rate = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_preferences);
        preferences_layout->addRow(m_lbl_frame_rate, m_cmb_frame_rate);
        QFormLayout* advanced_layout{ new QFormLayout(m_grp_advanced) };
        advanced_layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        layout->addWidget(m_grp_advanced);
        m_lbl_arguments = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_advanced);
        advanced_layout->addRow(m_lbl_arguments, m_txt_arguments);
        m_lbl_arguments_description = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_advanced);
        m_lbl_arguments_description->setWordWrap(true);
        advanced_layout->addRow(m_lbl_arguments_description);
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
        m_txt_arguments->setText(QString::fromStdString(options.ytdlp_arguments()));
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed,
                this,
                &SettingsDiscoveryPage::translate);
        connect(m_chk_media_id, &QCheckBox::toggled, this, &SettingsDiscoveryPage::save);
        connect(m_chk_limit_characters, &QCheckBox::toggled, this, &SettingsDiscoveryPage::save);
        connect(m_chk_super_resolutions, &QCheckBox::toggled, this, &SettingsDiscoveryPage::save);
        connect(m_chk_auto_generated_subtitles, &QCheckBox::toggled, this, &SettingsDiscoveryPage::save);
        connect(m_cmb_audio_codec, &QComboBox::currentIndexChanged, this, &SettingsDiscoveryPage::save);
        connect(m_cmb_video_codec, &QComboBox::currentIndexChanged, this, &SettingsDiscoveryPage::save);
        connect(m_cmb_subtitle_format, &QComboBox::currentIndexChanged, this, &SettingsDiscoveryPage::save);
        connect(m_cmb_frame_rate, &QComboBox::currentIndexChanged, this, &SettingsDiscoveryPage::save);
        connect(m_txt_arguments, &QLineEdit::textChanged, this, &SettingsDiscoveryPage::save);
    }

    void SettingsDiscoveryPage::save()
    {
        configuration::Configuration& configuration{ application::State::instance().configuration() };
        discovery::Options options{ configuration.discovery_options() };
        options.set_include_media_id_in_title(m_chk_media_id->isChecked());
        options.set_limit_characters(m_chk_limit_characters->isChecked());
        options.set_include_super_resolutions(m_chk_super_resolutions->isChecked());
        options.set_include_auto_generated_subtitles(m_chk_auto_generated_subtitles->isChecked());
        options.set_preferred_audio_codec(static_cast<audio::Codec>(m_cmb_audio_codec->currentIndex()));
        options.set_preferred_video_codec(static_cast<video::Codec>(m_cmb_video_codec->currentIndex()));
        options.set_preferred_subtitle_format(static_cast<subtitle::Type>(m_cmb_subtitle_format->currentIndex()));
        options.set_preferred_frame_rate(static_cast<video::FrameRate>(m_cmb_frame_rate->currentIndex()));
        options.set_ytdlp_arguments(m_txt_arguments->text().toStdString());
        configuration.set_discovery_options(options);
    }

    void SettingsDiscoveryPage::translate()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        QSignalBlocker audio_codec_blocker{ m_cmb_audio_codec };
        QSignalBlocker video_codec_blocker{ m_cmb_video_codec };
        QSignalBlocker subtitle_format_blocker{ m_cmb_subtitle_format };
        QSignalBlocker frame_rate_blocker{ m_cmb_frame_rate };
        int audio_codec_index{ m_cmb_audio_codec->currentIndex() };
        int video_codec_index{ m_cmb_video_codec->currentIndex() };
        int subtitle_format_index{ m_cmb_subtitle_format->currentIndex() };
        int frame_rate_index{ m_cmb_frame_rate->currentIndex() };
        m_grp_process->setTitle(translator._("Process"));
        m_chk_media_id->setText(translator._("Include Media Id in Title"));
        m_lbl_media_id->setText(translator._("Add the media entry's yt-dlp id to its default title."));
        m_chk_limit_characters->setText(translator._("Limit Filename Characters"));
        m_lbl_limit_characters->setText(translator._("Restrict filename characters to those supported only by Windows."));
        m_chk_super_resolutions->setText(translator._("Show Super Resolution Formats"));
        m_lbl_super_resolutions->setText(
                translator._("Super resolutions are resolutions that have been enhanced with artificial intelligence."));
        m_chk_auto_generated_subtitles->setText(translator._("Show Auto-Generated Subtitles"));
        m_grp_preferences->setTitle(translator._("Preferences"));
        m_grp_preferences->setDescription(translator._("Prefer these codecs and options when showing formats for download."));
        m_lbl_audio_codec->setText(QString::fromStdString(translator._("{}:", translator._("Audio Codec"))));
        m_cmb_audio_codec->clear();
        m_cmb_audio_codec->addItems({ translator._("Any"),
                                      translator._("FLAC"),
                                      translator._("WAV"),
                                      translator._("Opus"),
                                      translator._("AAC"),
                                      translator._("MP4A"),
                                      translator._("MP3") });
        m_cmb_audio_codec->setCurrentIndex(std::max(audio_codec_index, 0));
        m_lbl_video_codec->setText(QString::fromStdString(translator._("{}:", translator._("Video Codec"))));
        m_cmb_video_codec->clear();
        m_cmb_video_codec->addItems({ translator._("Any"),
                                      translator._("H.265"),
                                      translator._("VP9"),
                                      translator._("AV1"),
                                      translator._("H.264") });
        m_cmb_video_codec->setCurrentIndex(std::max(video_codec_index, 0));
        m_lbl_subtitle_format->setText(QString::fromStdString(translator._("{}:", translator._("Subtitle Format"))));
        m_cmb_subtitle_format->clear();
        m_cmb_subtitle_format->addItems(
                { translator._("Any"), translator._("VTT"), translator._("SRT"), translator._("ASS"), translator._("LRC") });
        m_cmb_subtitle_format->setCurrentIndex(std::max(subtitle_format_index, 0));
        m_lbl_frame_rate->setText(QString::fromStdString(translator._("{}:", translator._("Frame Rate"))));
        m_cmb_frame_rate->clear();
        m_cmb_frame_rate->addItems(
                { translator._("Any"), translator._("24 FPS"), translator._("30 FPS"), translator._("60 FPS") });
        m_cmb_frame_rate->setCurrentIndex(std::max(frame_rate_index, 0));
        m_grp_advanced->setTitle(translator._("Advanced"));
        m_lbl_arguments->setText(QString::fromStdString(translator._("{}:", translator._("Arguments"))));
        m_lbl_arguments_description->setText(
                translator._("These arguments will be appended to yt-dlp during the discovery phase."));
    }
}
