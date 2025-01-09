#ifndef QTHELPERS_H
#define QTHELPERS_H

#include <functional>
#include <string>
#include <vector>
#include <QComboBox>

namespace Nickvision::TubeConverter::Qt::Helpers::QtHelpers
{
    /**
     * @brief Runs the function on the main UI thread.
     * @param function The function to run
     */
    void dispatchToMainThread(const std::function<void()>& function);
    /**
     * @brief Sets the items of a QComboBox from a vector of strings.
     * @param comboBox The QComboBox to set the items of
     * @param items The items to set
     * @param selected An option string that should be selected
     */
    void setComboBoxItems(QComboBox* comboBox, const std::vector<std::string>& items, const std::string& selected = "");
}

#endif //QTHELPERS_H