#include "views/settings_downloads_page.h"
#include <QApplication>
#include <QCheckBox>
#include <QDesktopServices>
#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QObject>
#include <QPalette>
#include <QScrollArea>
#include <QSpinBox>
#include <QString>
#include <QToolButton>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>
#include <oclero/qlementine/Common.hpp>
#include <oclero/qlementine/icons/Icons16.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/widgets/GroupBox.hpp>
#include <oclero/qlementine/widgets/Label.hpp>
#include "app.h"
#include "application.h"
#include "aria2.h"
#include "configuration.h"
#include "download.h"
#include "helpers/translation_notifier.h"
#include "translation.h"

namespace parabolic::qt::views
{
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
        m_txt_arguments->setText(QString::fromStdString(download_options.ytdlp_arguments()));
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed,
                this,
                &SettingsDownloadsPage::translate);
        connect(m_spn_max_downloads, &QSpinBox::valueChanged, this, &SettingsDownloadsPage::save);
        connect(m_chk_overwrite_existing_files, &QCheckBox::toggled, this, &SettingsDownloadsPage::save);
        connect(m_chk_prevent_suspend, &QCheckBox::toggled, this, &SettingsDownloadsPage::save);
        connect(m_chk_use_aria, &QCheckBox::toggled, this, &SettingsDownloadsPage::save);
        connect(m_btn_learn_more, &QToolButton::clicked, this, &SettingsDownloadsPage::learn_more);
        connect(m_spn_aria_connections, &QSpinBox::valueChanged, this, &SettingsDownloadsPage::save);
        connect(m_spn_aria_split_size, &QSpinBox::valueChanged, this, &SettingsDownloadsPage::save);
        connect(m_txt_arguments, &QLineEdit::textChanged, this, &SettingsDownloadsPage::save);
    }

    void SettingsDownloadsPage::learn_more()
    {
        QDesktopServices::openUrl(QUrl("https://aria2.github.io"));
    }

    void SettingsDownloadsPage::save()
    {
        m_spn_aria_connections->setEnabled(m_chk_use_aria->isChecked());
        m_spn_aria_split_size->setEnabled(m_chk_use_aria->isChecked());
        configuration::Configuration& configuration{ application::State::instance().configuration() };
        download::Options download_options{ configuration.download_options() };
        download_options.set_max_number_of_active_downloads(m_spn_max_downloads->value());
        download_options.set_overwrite_existing_files(m_chk_overwrite_existing_files->isChecked());
        download_options.set_prevent_suspend(m_chk_prevent_suspend->isChecked());
        download_options.set_ytdlp_arguments(m_txt_arguments->text().toStdString());
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
        m_lbl_max_downloads->setText(QString::fromStdString(translator._("{}:", translator._("Maximum Active Downloads"))));
        m_chk_overwrite_existing_files->setText(translator._("Overwrite Existing Files"));
        m_chk_prevent_suspend->setText(translator._("Prevent Sleep During Downloads"));
        m_grp_aria->setTitle(translator._("aria2"));
        m_grp_aria->setDescription(translator._("An alternative downloader that may be faster in some regions compared to "
                                                "yt-dlp's native downloader."));
        m_chk_use_aria->setText(translator._("Use aria2"));
        m_btn_learn_more->setToolTip(translator._("Learn More"));
        m_lbl_aria_connections->setText(
                QString::fromStdString(translator._("{}:", translator._("Maximum Connections Per Server"))));
        m_lbl_aria_split_size->setText(QString::fromStdString(translator._("{}:", translator._("Minimum Split Size (MiB)"))));
        m_grp_advanced->setTitle(translator._("Advanced"));
        m_lbl_arguments->setText(QString::fromStdString(translator._("{}:", translator._("Arguments"))));
        m_lbl_arguments_description->setText(
                translator._("These arguments will be appended to yt-dlp during the download phase."));
    }
}
