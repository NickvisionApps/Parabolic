#ifndef SETTINGSDOWNLOADSPAGE_H
#define SETTINGSDOWNLOADSPAGE_H

#include <QCheckBox>
#include <QLineEdit>
#include <QScrollArea>
#include <QSpinBox>
#include <QToolButton>
#include <QWidget>
#include <oclero/qlementine/widgets/GroupBox.hpp>
#include <oclero/qlementine/widgets/Label.hpp>

namespace parabolic::qt::views
{
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
}

#endif
