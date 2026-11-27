#include "views/settings_postprocessing_page.h"
#include <QApplication>
#include <QCheckBox>
#include <QDialog>
#include <QFrame>
#include <QHBoxLayout>
#include <QListWidget>
#include <QMessageBox>
#include <QObject>
#include <QPalette>
#include <QScrollArea>
#include <QSpinBox>
#include <QString>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <algorithm>
#include <cstddef>
#include <oclero/qlementine/Common.hpp>
#include <oclero/qlementine/icons/Icons16.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/widgets/GroupBox.hpp>
#include <oclero/qlementine/widgets/Label.hpp>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "app.h"
#include "application.h"
#include "configuration.h"
#include "controls/postprocessing_argument_dialog.h"
#include "helpers/translation_notifier.h"
#include "postprocessing.h"
#include "translation.h"

namespace parabolic::qt::views
{
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
        configuration::Configuration& configuration{ application::State::instance().configuration() };
        QVBoxLayout* metadata_layout{ new QVBoxLayout(m_grp_metadata) };
        layout->addWidget(m_grp_metadata);
        metadata_layout->addWidget(m_chk_embed_metadata);
        metadata_layout->addWidget(m_chk_remove_source_data);
        m_lbl_remove_source_data = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_metadata);
        m_lbl_remove_source_data->setWordWrap(true);
        m_lbl_remove_source_data->setContentsMargins(24, 0, 0, 0);
        metadata_layout->addWidget(m_lbl_remove_source_data);
        QVBoxLayout* thumbnails_layout{ new QVBoxLayout(m_grp_thumbnails) };
        layout->addWidget(m_grp_thumbnails);
        thumbnails_layout->addWidget(m_chk_embed_thumbnails);
        m_lbl_embed_thumbnails = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_thumbnails);
        m_lbl_embed_thumbnails->setWordWrap(true);
        m_lbl_embed_thumbnails->setContentsMargins(24, 0, 0, 0);
        thumbnails_layout->addWidget(m_lbl_embed_thumbnails);
        thumbnails_layout->addWidget(m_chk_crop_audio_thumbnails);
        m_lbl_crop_audio_thumbnails =
                new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_thumbnails);
        m_lbl_crop_audio_thumbnails->setWordWrap(true);
        m_lbl_crop_audio_thumbnails->setContentsMargins(24, 0, 0, 0);
        thumbnails_layout->addWidget(m_lbl_crop_audio_thumbnails);
        QVBoxLayout* chapters_layout{ new QVBoxLayout(m_grp_chapters) };
        layout->addWidget(m_grp_chapters);
        chapters_layout->addWidget(m_chk_embed_chapters);
        QVBoxLayout* subtitles_layout{ new QVBoxLayout(m_grp_subtitles) };
        layout->addWidget(m_grp_subtitles);
        subtitles_layout->addWidget(m_chk_embed_subtitles);
        m_lbl_embed_subtitles = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_subtitles);
        m_lbl_embed_subtitles->setWordWrap(true);
        m_lbl_embed_subtitles->setContentsMargins(24, 0, 0, 0);
        subtitles_layout->addWidget(m_lbl_embed_subtitles);
        QVBoxLayout* ffmpeg_layout{ new QVBoxLayout(m_grp_ffmpeg) };
        layout->addWidget(m_grp_ffmpeg);
        m_lbl_threads = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_ffmpeg);
        m_spn_threads->setRange(1, static_cast<int>(std::max(1U, std::thread::hardware_concurrency())));
        QHBoxLayout* threads_row{ new QHBoxLayout() };
        threads_row->addWidget(m_lbl_threads);
        threads_row->addWidget(m_spn_threads);
        threads_row->addStretch();
        ffmpeg_layout->addLayout(threads_row);
        m_lbl_threads_description = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_ffmpeg);
        m_lbl_threads_description->setWordWrap(true);
        ffmpeg_layout->addWidget(m_lbl_threads_description);
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
                &helpers::TranslationNotifier::translation_changed,
                this,
                &SettingsPostprocessingPage::translate);
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
        controls::PostprocessingArgumentDialog dialog{ { "", postprocessing::Step::None, postprocessing::Executable::None, "" },
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
        controls::PostprocessingArgumentDialog dialog{ m_arguments[static_cast<std::size_t>(row)], names, this };
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
        if (QMessageBox::question(this,
                                  translator._("Remove Argument"),
                                  QString::fromStdString(translator._("Are you sure you want to remove \"{}\"?",
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
        configuration::Configuration& configuration{ application::State::instance().configuration() };
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
        m_lbl_remove_source_data->setText(translator._("Clear metadata fields containing identifying download information."));
        m_grp_thumbnails->setTitle(translator._("Thumbnails"));
        m_chk_embed_thumbnails->setText(translator._("Embed Thumbnails"));
        m_lbl_embed_thumbnails->setText(translator._("If the file type does not support embedding, the thumbnail will "
                                                     "be written to a separate image file."));
        m_chk_crop_audio_thumbnails->setText(translator._("Crop Audio Thumbnails"));
        m_lbl_crop_audio_thumbnails->setText(translator._("Crop thumbnails of audio files to squares."));
        m_grp_chapters->setTitle(translator._("Chapters"));
        m_chk_embed_chapters->setText(translator._("Embed Chapters"));
        m_grp_subtitles->setTitle(translator._("Subtitles"));
        m_chk_embed_subtitles->setText(translator._("Embed Subtitles"));
        m_lbl_embed_subtitles->setText(translator._(
                "If disabled or if embedding is not supported, downloaded subtitles will be saved to separate files."));
        m_grp_ffmpeg->setTitle(translator._("FFmpeg"));
        m_lbl_threads->setText(QString::fromStdString(translator._("{}:", translator._("Threads"))));
        m_lbl_threads_description->setText(translator._("Limit the number of threads used by ffmpeg."));
        m_grp_arguments->setTitle(translator._("Arguments"));
        m_grp_arguments->setDescription(
                translator._("Arguments will be shown for selection in the download discovery dialog."));
        m_btn_add->setToolTip(translator._("Add"));
        m_btn_edit->setToolTip(translator._("Edit"));
        m_btn_remove->setToolTip(translator._("Remove"));
    }
}
