#ifndef QTHELPERS_H
#define QTHELPERS_H

#include <functional>
#include <string>
#include <vector>
#include <QComboBox>

namespace Nickvision::TubeConverter::QT::Helpers::QTHelpers
{
    /**
     * @brief Runs the function on the main UI thread.
     * @param function The function to run
     */
    void dispatchToMainThread(const std::function<void()>& function);
    /**
     * @brief Sets the items of a QComboBox from a vector of strings.
     * @param comboBox The QComboBox to set the items of
     */
    void setComboBoxItems(QComboBox* comboBox, const std::vector<std::string>& items);
}

#endif //QTHELPERS_H