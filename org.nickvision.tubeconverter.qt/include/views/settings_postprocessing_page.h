#ifndef SETTINGSPOSTPROCESSINGPAGE_H
#define SETTINGSPOSTPROCESSINGPAGE_H

#include <QCheckBox>
#include <QListWidget>
#include <QScrollArea>
#include <QSpinBox>
#include <QToolButton>
#include <QWidget>
#include <oclero/qlementine/widgets/GroupBox.hpp>
#include <oclero/qlementine/widgets/Label.hpp>
#include <vector>
#include "postprocessing.h"

namespace parabolic::qt::views
{
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
}

#endif
