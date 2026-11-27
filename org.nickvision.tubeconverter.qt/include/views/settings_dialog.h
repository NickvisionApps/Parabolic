#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QWidget>
#include <oclero/qlementine/widgets/NavigationView.hpp>

namespace parabolic::qt::views
{
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
