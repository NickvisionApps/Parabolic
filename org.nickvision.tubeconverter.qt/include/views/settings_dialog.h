#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QWidget>
#include <oclero/qlementine/widgets/NavigationView.hpp>

namespace parabolic::qt::views
{
    class SettingsDialog : public QDialog
    {
    public:
        SettingsDialog(QWidget* parent = nullptr);
        ~SettingsDialog() override = default;
        SettingsDialog(const SettingsDialog&) = delete;
        SettingsDialog(SettingsDialog&&) = delete;
        SettingsDialog& operator=(const SettingsDialog&) = delete;
        SettingsDialog& operator=(SettingsDialog&&) = delete;

    private:
        oclero::qlementine::NavigationView* m_navigation_view{ nullptr };
    };
}

#endif
