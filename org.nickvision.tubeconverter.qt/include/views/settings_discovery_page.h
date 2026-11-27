#ifndef SETTINGSDISCOVERYPAGE_H
#define SETTINGSDISCOVERYPAGE_H

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QScrollArea>
#include <QWidget>
#include <oclero/qlementine/widgets/GroupBox.hpp>
#include <oclero/qlementine/widgets/Label.hpp>

namespace parabolic::qt::views
{
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
}

#endif
