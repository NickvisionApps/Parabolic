#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QToolButton>
#include <QWidget>
#include <oclero/qlementine/widgets/GroupBox.hpp>
#include <oclero/qlementine/widgets/Label.hpp>
#include <oclero/qlementine/widgets/NavigationView.hpp>
#include <string>
#include <vector>
#include "postprocessing.h"

namespace parabolic::qt::views
{
    class PostprocessingArgumentDialog : public QDialog
    {
        Q_OBJECT

    public:
        PostprocessingArgumentDialog(const postprocessing::Argument& argument,
                                     const std::vector<std::string>& taken_names,
                                     QWidget* parent = nullptr);
        ~PostprocessingArgumentDialog() override = default;
        PostprocessingArgumentDialog(const PostprocessingArgumentDialog&) = delete;
        PostprocessingArgumentDialog(PostprocessingArgumentDialog&&) = delete;
        PostprocessingArgumentDialog& operator=(const PostprocessingArgumentDialog&) = delete;
        PostprocessingArgumentDialog& operator=(PostprocessingArgumentDialog&&) = delete;
        [[nodiscard]] postprocessing::Argument argument() const;

    private:
        Q_SLOT void translate();
        Q_SLOT void validate();
        std::vector<std::string> m_taken_names;
        oclero::qlementine::Label* m_lbl_name{ nullptr };
        QLineEdit* m_txt_name{ nullptr };
        oclero::qlementine::Label* m_lbl_step{ nullptr };
        QComboBox* m_cmb_step{ nullptr };
        oclero::qlementine::Label* m_lbl_executable{ nullptr };
        QComboBox* m_cmb_executable{ nullptr };
        oclero::qlementine::Label* m_lbl_arguments{ nullptr };
        QLineEdit* m_txt_arguments{ nullptr };
        QDialogButtonBox* m_buttons{ nullptr };
    };

    class SettingsGeneralPage : public QScrollArea
    {
        Q_OBJECT

    public:
        SettingsGeneralPage(QWidget* parent = nullptr);
        ~SettingsGeneralPage() override = default;
        SettingsGeneralPage(const SettingsGeneralPage&) = delete;
        SettingsGeneralPage(SettingsGeneralPage&&) = delete;
        SettingsGeneralPage& operator=(const SettingsGeneralPage&) = delete;
        SettingsGeneralPage& operator=(SettingsGeneralPage&&) = delete;

    private:
        Q_SLOT void change_language();
        Q_SLOT void change_theme();
        Q_SLOT void save();
        Q_SLOT void translate();
        std::vector<std::string> m_languages;
        oclero::qlementine::GroupBox* m_grp_user_interface{ nullptr };
        oclero::qlementine::Label* m_lbl_theme{ nullptr };
        QComboBox* m_cmb_theme{ nullptr };
        oclero::qlementine::Label* m_lbl_language{ nullptr };
        QComboBox* m_cmb_language{ nullptr };
        oclero::qlementine::GroupBox* m_grp_updates{ nullptr };
        QCheckBox* m_chk_automatic_updates{ nullptr };
        QCheckBox* m_chk_preview_updates{ nullptr };
        oclero::qlementine::Label* m_lbl_preview_updates{ nullptr };
        oclero::qlementine::GroupBox* m_grp_history{ nullptr };
        oclero::qlementine::Label* m_lbl_history_length{ nullptr };
        QComboBox* m_cmb_history_length{ nullptr };
    };

    class SettingsDiscoveryPage : public QScrollArea
    {
        Q_OBJECT

    public:
        SettingsDiscoveryPage(QWidget* parent = nullptr);
        ~SettingsDiscoveryPage() override = default;
        SettingsDiscoveryPage(const SettingsDiscoveryPage&) = delete;
        SettingsDiscoveryPage(SettingsDiscoveryPage&&) = delete;
        SettingsDiscoveryPage& operator=(const SettingsDiscoveryPage&) = delete;
        SettingsDiscoveryPage& operator=(SettingsDiscoveryPage&&) = delete;

    private:
        Q_SLOT void save();
        Q_SLOT void translate();
        oclero::qlementine::GroupBox* m_grp_process{ nullptr };
        QCheckBox* m_chk_media_id{ nullptr };
        oclero::qlementine::Label* m_lbl_media_id{ nullptr };
        QCheckBox* m_chk_limit_characters{ nullptr };
        oclero::qlementine::Label* m_lbl_limit_characters{ nullptr };
        QCheckBox* m_chk_super_resolutions{ nullptr };
        oclero::qlementine::Label* m_lbl_super_resolutions{ nullptr };
        QCheckBox* m_chk_auto_generated_subtitles{ nullptr };
        oclero::qlementine::GroupBox* m_grp_preferences{ nullptr };
        oclero::qlementine::Label* m_lbl_audio_codec{ nullptr };
        QComboBox* m_cmb_audio_codec{ nullptr };
        oclero::qlementine::Label* m_lbl_video_codec{ nullptr };
        QComboBox* m_cmb_video_codec{ nullptr };
        oclero::qlementine::Label* m_lbl_subtitle_format{ nullptr };
        QComboBox* m_cmb_subtitle_format{ nullptr };
        oclero::qlementine::Label* m_lbl_frame_rate{ nullptr };
        QComboBox* m_cmb_frame_rate{ nullptr };
        oclero::qlementine::GroupBox* m_grp_advanced{ nullptr };
        oclero::qlementine::Label* m_lbl_arguments{ nullptr };
        QLineEdit* m_txt_arguments{ nullptr };
        oclero::qlementine::Label* m_lbl_arguments_description{ nullptr };
    };

    class SettingsDownloadsPage : public QScrollArea
    {
        Q_OBJECT

    public:
        SettingsDownloadsPage(QWidget* parent = nullptr);
        ~SettingsDownloadsPage() override = default;
        SettingsDownloadsPage(const SettingsDownloadsPage&) = delete;
        SettingsDownloadsPage(SettingsDownloadsPage&&) = delete;
        SettingsDownloadsPage& operator=(const SettingsDownloadsPage&) = delete;
        SettingsDownloadsPage& operator=(SettingsDownloadsPage&&) = delete;

    private:
        Q_SLOT void learn_more();
        Q_SLOT void save();
        Q_SLOT void translate();
        oclero::qlementine::GroupBox* m_grp_process{ nullptr };
        oclero::qlementine::Label* m_lbl_max_downloads{ nullptr };
        QSpinBox* m_spn_max_downloads{ nullptr };
        QCheckBox* m_chk_overwrite_existing_files{ nullptr };
        QCheckBox* m_chk_prevent_suspend{ nullptr };
        oclero::qlementine::GroupBox* m_grp_aria{ nullptr };
        QCheckBox* m_chk_use_aria{ nullptr };
        QToolButton* m_btn_learn_more{ nullptr };
        oclero::qlementine::Label* m_lbl_aria_connections{ nullptr };
        QSpinBox* m_spn_aria_connections{ nullptr };
        oclero::qlementine::Label* m_lbl_aria_split_size{ nullptr };
        QSpinBox* m_spn_aria_split_size{ nullptr };
        oclero::qlementine::GroupBox* m_grp_advanced{ nullptr };
        oclero::qlementine::Label* m_lbl_arguments{ nullptr };
        QLineEdit* m_txt_arguments{ nullptr };
        oclero::qlementine::Label* m_lbl_arguments_description{ nullptr };
    };

    class SettingsExtractorPage : public QScrollArea
    {
        Q_OBJECT

    public:
        SettingsExtractorPage(QWidget* parent = nullptr);
        ~SettingsExtractorPage() override = default;
        SettingsExtractorPage(const SettingsExtractorPage&) = delete;
        SettingsExtractorPage(SettingsExtractorPage&&) = delete;
        SettingsExtractorPage& operator=(const SettingsExtractorPage&) = delete;
        SettingsExtractorPage& operator=(SettingsExtractorPage&&) = delete;

    private:
        Q_SLOT void browse_cookies_file();
        Q_SLOT void save();
        Q_SLOT void translate();
        oclero::qlementine::GroupBox* m_grp_process{ nullptr };
        QCheckBox* m_chk_part_files{ nullptr };
        oclero::qlementine::Label* m_lbl_part_files{ nullptr };
        QCheckBox* m_chk_sponsor_block{ nullptr };
        oclero::qlementine::Label* m_lbl_sponsor_block{ nullptr };
        QCheckBox* m_chk_translate_metadata{ nullptr };
        oclero::qlementine::Label* m_lbl_translate_metadata{ nullptr };
        oclero::qlementine::GroupBox* m_grp_speed{ nullptr };
        QCheckBox* m_chk_speed_limit{ nullptr };
        oclero::qlementine::Label* m_lbl_speed_limit{ nullptr };
        QSpinBox* m_spn_speed_limit{ nullptr };
        oclero::qlementine::GroupBox* m_grp_proxy{ nullptr };
        oclero::qlementine::Label* m_lbl_proxy_url{ nullptr };
        QLineEdit* m_txt_proxy_url{ nullptr };
        oclero::qlementine::GroupBox* m_grp_cookies{ nullptr };
        oclero::qlementine::Label* m_lbl_cookies_browser{ nullptr };
        QComboBox* m_cmb_cookies_browser{ nullptr };
        oclero::qlementine::Label* m_lbl_cookies_browser_description{ nullptr };
        oclero::qlementine::Label* m_lbl_cookies_file{ nullptr };
        QLineEdit* m_txt_cookies_path{ nullptr };
        QPushButton* m_btn_cookies_browse{ nullptr };
        QPushButton* m_btn_cookies_clear{ nullptr };
    };

    class SettingsPostprocessingPage : public QScrollArea
    {
        Q_OBJECT

    public:
        SettingsPostprocessingPage(QWidget* parent = nullptr);
        ~SettingsPostprocessingPage() override = default;
        SettingsPostprocessingPage(const SettingsPostprocessingPage&) = delete;
        SettingsPostprocessingPage(SettingsPostprocessingPage&&) = delete;
        SettingsPostprocessingPage& operator=(const SettingsPostprocessingPage&) = delete;
        SettingsPostprocessingPage& operator=(SettingsPostprocessingPage&&) = delete;

    private:
        Q_SLOT void add_argument();
        Q_SLOT void edit_argument();
        Q_SLOT void remove_argument();
        Q_SLOT void save();
        Q_SLOT void translate();
        std::vector<postprocessing::Argument> m_arguments;
        oclero::qlementine::GroupBox* m_grp_metadata{ nullptr };
        QCheckBox* m_chk_embed_metadata{ nullptr };
        QCheckBox* m_chk_remove_source_data{ nullptr };
        oclero::qlementine::Label* m_lbl_remove_source_data{ nullptr };
        oclero::qlementine::GroupBox* m_grp_thumbnails{ nullptr };
        QCheckBox* m_chk_embed_thumbnails{ nullptr };
        oclero::qlementine::Label* m_lbl_embed_thumbnails{ nullptr };
        QCheckBox* m_chk_crop_audio_thumbnails{ nullptr };
        oclero::qlementine::Label* m_lbl_crop_audio_thumbnails{ nullptr };
        oclero::qlementine::GroupBox* m_grp_chapters{ nullptr };
        QCheckBox* m_chk_embed_chapters{ nullptr };
        oclero::qlementine::GroupBox* m_grp_subtitles{ nullptr };
        QCheckBox* m_chk_embed_subtitles{ nullptr };
        oclero::qlementine::Label* m_lbl_embed_subtitles{ nullptr };
        oclero::qlementine::GroupBox* m_grp_ffmpeg{ nullptr };
        oclero::qlementine::Label* m_lbl_threads{ nullptr };
        QSpinBox* m_spn_threads{ nullptr };
        oclero::qlementine::Label* m_lbl_threads_description{ nullptr };
        oclero::qlementine::GroupBox* m_grp_arguments{ nullptr };
        QToolButton* m_btn_add{ nullptr };
        QToolButton* m_btn_edit{ nullptr };
        QToolButton* m_btn_remove{ nullptr };
        QListWidget* m_lst_arguments{ nullptr };
    };

    class SettingsDialog : public QDialog
    {
        Q_OBJECT

    public:
        SettingsDialog(QWidget* parent = nullptr);
        ~SettingsDialog() override = default;
        SettingsDialog(const SettingsDialog&) = delete;
        SettingsDialog(SettingsDialog&&) = delete;
        SettingsDialog& operator=(const SettingsDialog&) = delete;
        SettingsDialog& operator=(SettingsDialog&&) = delete;

    private:
        Q_SLOT void translate();
        oclero::qlementine::NavigationView* m_navigation_view{ nullptr };
    };
}

#endif
