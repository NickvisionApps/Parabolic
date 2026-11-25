#include "views/settings_dialog.h"
#include <QDialog>
#include <QWidget>
#include "application.h"
#include "translation.h"

namespace parabolic::qt::views
{
    SettingsDialog::SettingsDialog(QWidget* parent)
        : QDialog{ parent }
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        setWindowTitle(translator._("Settings"));
        setModal(true);
        setFixedSize(600, 400);
    }
}
