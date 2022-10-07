#include "preferencesdialogcontroller.hpp"

using namespace NickvisionTubeConverter::Controllers;
using namespace NickvisionTubeConverter::Models;

PreferencesDialogController::PreferencesDialogController(Configuration& configuration) : m_configuration{ configuration }
{

}

int PreferencesDialogController::getThemeAsInt() const
{
    return static_cast<int>(m_configuration.getTheme());
}

void PreferencesDialogController::setTheme(int theme)
{
    m_configuration.setTheme(static_cast<Theme>(theme));
}

bool PreferencesDialogController::getIsFirstTimeOpen() const
{
    return m_configuration.getIsFirstTimeOpen();
}

void PreferencesDialogController::setIsFirstTimeOpen(bool isFirstTimeOpen)
{
    m_configuration.setIsFirstTimeOpen(isFirstTimeOpen);
}

void PreferencesDialogController::saveConfiguration() const
{
    m_configuration.save();
}
