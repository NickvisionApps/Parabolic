#ifndef HISTORYPANE_H
#define HISTORYPANE_H

#include <vector>
#include <QDockWidget>
#include <QFrame>
#include <QWidget>
#include "controls/historyrow.h"
#include "models/historicdownload.h"

namespace Ui { class HistoryPane; }

namespace Nickvision::TubeConverter::Qt::Controls
{
    /**
     * @brief A side pane for working with download history.
     */
    class HistoryPane : public QDockWidget
    {
    Q_OBJECT

    public:
        /**
         * @brief Constructs a HistoryPane.
         * @param parent The parent widget
         */
        HistoryPane(QWidget* parent = nullptr);
        /**
         * @brief Destructs a HistoryPane.
         */
        ~HistoryPane();
        /**
         * @brief Updates the history shown in the pane.
         * @param history The new history
         */
        void update(const std::vector<Shared::Models::HistoricDownload>& history);

    Q_SIGNALS:
        /**
         * @brief Emitted when the download button on a history row is clicked.
         * @param url THe url of the historic download
         */
        void downloadAgain(const std::string& url);
        /**
         * @brief Emitted when the delete button on a history row is clicked.
         * @param download The historic download
         */
        void deleteItem(const Shared::Models::HistoricDownload& download);

    private:
        /**
         * @brief Clears the history from the pane.
         */
        void clear();
        Ui::HistoryPane* m_ui;
        std::vector<HistoryRow*> m_rows;
        std::vector<QFrame*> m_lines;
    };
}

#endif //HISTORYPANE_H
