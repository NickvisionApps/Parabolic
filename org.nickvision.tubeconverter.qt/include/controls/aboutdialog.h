#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <string>
#include <QDialog>
#include <libnick/app/appinfo.h>

namespace Ui { class AboutDialog; }

namespace Nickvision::TubeConverter::Qt::Controls
{
    /**
     * @brief A dialog for displaying information about the application.
     */
    class AboutDialog : public QDialog
    {
    Q_OBJECT

    public:
        /**
         * @brief Constructs an AboutDialog.
         * @param appInfo The AppInfo object for the application
         * @param debugInfo The debug information for the application
         * @param parent The parent widget
         */
        AboutDialog(const App::AppInfo& appInfo, const std::string& debugInfo, QWidget* parent = nullptr);
        /**
         * @brief Destructs an AboutDialog.
         */
        ~AboutDialog();

    private:
        Ui::AboutDialog* m_ui;
    };
}

#endif //ABOUTDIALOG_H