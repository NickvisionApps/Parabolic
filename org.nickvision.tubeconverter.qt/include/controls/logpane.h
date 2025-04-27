#ifndef LOGPANE_H
#define LOGPANE_H

#include <QDockWidget>
#include <QString>

namespace Ui { class LogPane; }

namespace Nickvision::TubeConverter::Qt::Controls
{
    /**
     * @brief A side pane for displaying download logs.
     */
    class LogPane : public QDockWidget
    {
    Q_OBJECT

    public:
        /**
         * @brief Constructs a LogPane.
         * @param parent The parent widget
         */
        LogPane(QWidget* parent = nullptr);
        /**
         * @brief Destructs a LogPane.
         */
        ~LogPane();
        /**
         * @brief Gets the id of the download who's log is being shown.
         * @return The id of the download who's log is being shown
         * @return -1 if no download's log is being shown
         */
        int id() const;
        /**
         * @brief Sets the id of the download who's log is being shown.
         * @param id The id of the download who's log is being shown
         */
        void setId(int id);
        /**
         * @brief Updates the log shown in the pane.
         * @param log The new log
         */
        void update(const QString& log);

    private Q_SLOTS:
        /**
         * @brief Handles when the pane's visibility is changed.
         * @param visible Whether or not the pane is visible
         */
        void onVisibilityChanged(bool visible);

    private:
        Ui::LogPane* m_ui;
        int m_id;
    };
}

#endif //LOGPANE_H
