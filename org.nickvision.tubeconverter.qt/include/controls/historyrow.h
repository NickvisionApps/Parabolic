#ifndef HISTORYROW_H
#define HISTORYROW_H

#include <QWidget>
#include "models/historicdownload.h"

namespace Ui { class HistoryRow; }

namespace Nickvision::TubeConverter::Qt::Controls
{
    /**
     * @brief A row widget for displaying a historic download.
     */
    class HistoryRow : public QWidget
    {
    Q_OBJECT

    public:
        /**
         * @brief HistoryRow Constructs a HistoryRow.
         * @param download The historic download
         * @param parent The parent widget
         */
        HistoryRow(const Shared::Models::HistoricDownload& download, QWidget* parent = nullptr);
        /**
         * @brief Destructs a HistoryRow.
         */
        ~HistoryRow();

    Q_SIGNALS:
        /**
         * @brief Emitted when the download button is clicked.
         * @param url THe url of the historic download
         */
        void downloadAgain(const std::string& url);
        /**
         * @brief Emitted when the delete button is clicked.
         * @param download The historic download
         */
        void deleteItem(const Shared::Models::HistoricDownload& download);

    private:
        Ui::HistoryRow* m_ui;
    };
}

#endif //HISTORYROW_H
