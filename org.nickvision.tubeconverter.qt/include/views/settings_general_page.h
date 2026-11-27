#ifndef SETTINGSGENERALPAGE_H
#define SETTINGSGENERALPAGE_H

#include <QCheckBox>
#include <QComboBox>
#include <QScrollArea>
#include <QWidget>
#include <oclero/qlementine/widgets/GroupBox.hpp>
#include <oclero/qlementine/widgets/Label.hpp>
#include <string>
#include <vector>

namespace parabolic::qt::views
{
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
}

#endif
