#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QWidget>
#include <string>
#include <vector>
#include "postprocessing.h"

namespace Ui
{
    class SettingsDialog;
}

namespace parabolic::qt::views
{
    class SettingsDialog : public QDialog
    {
        Q_OBJECT

    public:
        SettingsDialog(QWidget* parent = nullptr);
        ~SettingsDialog() override;
        SettingsDialog(const SettingsDialog&) = delete;
        SettingsDialog(SettingsDialog&&) = delete;
        SettingsDialog& operator=(const SettingsDialog&) = delete;
        SettingsDialog& operator=(SettingsDialog&&) = delete;

    private:
        Q_SLOT void add_argument();
        Q_SLOT void browse_cookies_file();
        Q_SLOT void change_language();
        Q_SLOT void change_theme();
        Q_SLOT void edit_argument();
        Q_SLOT void learn_more();
        Q_SLOT void remove_argument();
        Q_SLOT void save();
        Q_SLOT void translate();
        std::vector<std::string> m_languages;
        std::vector<postprocessing::Argument> m_arguments;
        Ui::SettingsDialog* m_ui{ nullptr };
    };
}

#endif
