#include "views/settings_extractor_page.h"
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QObject>
#include <QPalette>
#include <QPushButton>
#include <QScrollArea>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>
#include <algorithm>
#include <oclero/qlementine/Common.hpp>
#include <oclero/qlementine/icons/Icons16.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/widgets/GroupBox.hpp>
#include <oclero/qlementine/widgets/Label.hpp>
#include <optional>
#include "app.h"
#include "application.h"
#include "configuration.h"
#include "cookies.h"
#include "extractor.h"
#include "helpers/translation_notifier.h"
#include "translation.h"

namespace parabolic::qt::views
{
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
          m_txt_proxy_url{ new QLineEdit(m_grp_proxy) },
          m_grp_cookies{ new oclero::qlementine::GroupBox(this) },
          m_cmb_cookies_browser{ new QComboBox(m_grp_cookies) },
          m_txt_cookies_path{ new QLineEdit(m_grp_cookies) },
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
        configuration::Configuration& configuration{ application::State::instance().configuration() };
        QVBoxLayout* process_layout{ new QVBoxLayout(m_grp_process) };
        layout->addWidget(m_grp_process);
        process_layout->addWidget(m_chk_part_files);
        m_lbl_part_files = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_process);
        m_lbl_part_files->setWordWrap(true);
        m_lbl_part_files->setContentsMargins(24, 0, 0, 0);
        process_layout->addWidget(m_lbl_part_files);
        process_layout->addWidget(m_chk_sponsor_block);
        m_lbl_sponsor_block = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_process);
        m_lbl_sponsor_block->setWordWrap(true);
        m_lbl_sponsor_block->setContentsMargins(24, 0, 0, 0);
        process_layout->addWidget(m_lbl_sponsor_block);
        process_layout->addWidget(m_chk_translate_metadata);
        m_lbl_translate_metadata = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_process);
        m_lbl_translate_metadata->setWordWrap(true);
        m_lbl_translate_metadata->setContentsMargins(24, 0, 0, 0);
        process_layout->addWidget(m_lbl_translate_metadata);
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
        proxy_layout->addRow(m_lbl_proxy_url, m_txt_proxy_url);
        QFormLayout* cookies_layout{ new QFormLayout(m_grp_cookies) };
        cookies_layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        layout->addWidget(m_grp_cookies);
        m_lbl_cookies_browser = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_cookies);
        cookies_layout->addRow(m_lbl_cookies_browser, m_cmb_cookies_browser);
        m_lbl_cookies_browser_description =
                new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Caption, m_grp_cookies);
        m_lbl_cookies_browser_description->setWordWrap(true);
        cookies_layout->addRow(m_lbl_cookies_browser_description);
        m_lbl_cookies_file = new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, m_grp_cookies);
        m_txt_cookies_path->setReadOnly(true);
        m_btn_cookies_browse->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(
                oclero::qlementine::icons::iconPath(oclero::qlementine::icons::Icons16::File_Folder)));
        m_btn_cookies_clear->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(
                oclero::qlementine::icons::iconPath(oclero::qlementine::icons::Icons16::Action_Clear)));
        QHBoxLayout* cookies_file_row{ new QHBoxLayout() };
        cookies_file_row->addWidget(m_txt_cookies_path, 1);
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
        m_txt_proxy_url->setText(QString::fromStdString(options.proxy_url()));
        m_cmb_cookies_browser->setCurrentIndex(static_cast<int>(options.cookies_browser()));
        m_txt_cookies_path->setText(QString::fromStdString(options.cookies_path()));
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed,
                this,
                &SettingsExtractorPage::translate);
        connect(m_chk_part_files, &QCheckBox::toggled, this, &SettingsExtractorPage::save);
        connect(m_chk_sponsor_block, &QCheckBox::toggled, this, &SettingsExtractorPage::save);
        connect(m_chk_translate_metadata, &QCheckBox::toggled, this, &SettingsExtractorPage::save);
        connect(m_chk_speed_limit, &QCheckBox::toggled, this, &SettingsExtractorPage::save);
        connect(m_spn_speed_limit, &QSpinBox::valueChanged, this, &SettingsExtractorPage::save);
        connect(m_txt_proxy_url, &QLineEdit::textChanged, this, &SettingsExtractorPage::save);
        connect(m_cmb_cookies_browser, &QComboBox::currentIndexChanged, this, &SettingsExtractorPage::save);
        connect(m_txt_cookies_path, &QLineEdit::textChanged, this, &SettingsExtractorPage::save);
        connect(m_btn_cookies_browse, &QPushButton::clicked, this, &SettingsExtractorPage::browse_cookies_file);
        connect(m_btn_cookies_clear, &QPushButton::clicked, m_txt_cookies_path, &QLineEdit::clear);
    }

    void SettingsExtractorPage::browse_cookies_file()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        QString path{
            QFileDialog::getOpenFileName(this, translator._("Cookies File"), {}, translator._("Text Files (*.txt)"))
        };
        if (!path.isEmpty())
        {
            m_txt_cookies_path->setText(path);
        }
    }

    void SettingsExtractorPage::save()
    {
        m_spn_speed_limit->setEnabled(m_chk_speed_limit->isChecked());
        configuration::Configuration& configuration{ application::State::instance().configuration() };
        extractor::Options options{ configuration.extractor_options() };
        options.set_use_part_files(m_chk_part_files->isChecked());
        options.set_youtube_sponsor_block(m_chk_sponsor_block->isChecked());
        options.set_translate_metadata_and_chapters(m_chk_translate_metadata->isChecked());
        options.set_speed_limit(m_chk_speed_limit->isChecked() ? std::optional<int>{ m_spn_speed_limit->value() }
                                                               : std::nullopt);
        options.set_proxy_url(m_txt_proxy_url->text().toStdString());
        options.set_cookies_browser(static_cast<cookies::Browser>(m_cmb_cookies_browser->currentIndex()));
        options.set_cookies_path(m_txt_cookies_path->text().toStdString());
        configuration.set_extractor_options(options);
    }

    void SettingsExtractorPage::translate()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        QSignalBlocker cookies_browser_blocker{ m_cmb_cookies_browser };
        int cookies_browser_index{ m_cmb_cookies_browser->currentIndex() };
        m_grp_process->setTitle(translator._("Process"));
        m_chk_part_files->setText(translator._("Use Part Files"));
        m_lbl_part_files->setText(
                translator._("Download media in separate .part files instead of directly into the output file."));
        m_chk_sponsor_block->setText(translator._("Enable SponsorBlock for YouTube"));
        m_lbl_sponsor_block->setText(translator._("Try to remove sponsored segments from videos."));
        m_chk_translate_metadata->setText(translator._("Translate Metadata and Chapters"));
        m_lbl_translate_metadata->setText(translator._("Automatically translate embedded metadata and chapters using "
                                                       "the app's set language on supported sites and media."));
        m_grp_speed->setTitle(translator._("Speed"));
        m_chk_speed_limit->setText(translator._("Limit Download Speed"));
        m_lbl_speed_limit->setText(QString::fromStdString(translator._("{}:", translator._("Speed Limit (KiB/s)"))));
        m_grp_proxy->setTitle(translator._("Proxy"));
        m_lbl_proxy_url->setText(QString::fromStdString(translator._("{}:", translator._("URL"))));
        m_grp_cookies->setTitle(translator._("Cookies"));
        m_lbl_cookies_browser->setText(QString::fromStdString(translator._("{}:", translator._("Browser"))));
        m_cmb_cookies_browser->clear();
        m_cmb_cookies_browser->addItems({ translator._("None"),
                                          translator._("Brave"),
                                          translator._("Chrome"),
                                          translator._("Chromium"),
                                          translator._("Edge"),
                                          translator._("Firefox"),
                                          translator._("Opera"),
                                          translator._("Vivaldi"),
                                          translator._("Whale") });
        m_cmb_cookies_browser->setCurrentIndex(std::max(cookies_browser_index, 0));
        m_lbl_cookies_browser_description->setText(
                translator._("Selecting a browser other than \"None\" will use it instead of the cookies file."));
        m_lbl_cookies_file->setText(QString::fromStdString(translator._("{}:", translator._("File"))));
        m_btn_cookies_browse->setToolTip(translator._("Browse"));
        m_btn_cookies_clear->setToolTip(translator._("Clear"));
    }
}
