#ifndef SETTINGSEXTRACTORPAGE_H
#define SETTINGSEXTRACTORPAGE_H

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QWidget>
#include <oclero/qlementine/widgets/GroupBox.hpp>
#include <oclero/qlementine/widgets/Label.hpp>

namespace parabolic::qt::views
{
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
}

#endif
