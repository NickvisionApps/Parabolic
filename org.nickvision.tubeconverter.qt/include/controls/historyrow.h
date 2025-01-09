#ifndef HISTORYROW_H
#define HISTORYROW_H

#include <QWidget>
#include "models/historicdownload.h"

namespace Ui { class HistoryRow; }

namespace Nickvision::TubeConverter::Qt::Controls
{
    /**
     * @brief A row that displays and manages a historic download.
     */
    class HistoryRow : public QWidget
    {
    Q_OBJECT

    public:
        /**
         * @brief Constructs a HistoryRow.
         * @param download HistoricDownload
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
         * @param url The URL of the download
         */
        void downloadAgain(const std::string& url);
        /**
         * @brief Emitted when the delete button is clicked.
         * @param download The download to delete
         */
        void deleteItem(const Shared::Models::HistoricDownload& download);

    private Q_SLOTS:
        /**
         * @brief Opens the download in a media player.
         */
        void play();

    private:
        Ui::HistoryRow* m_ui;
        Shared::Models::HistoricDownload m_download;
    };
}

#endif //HISTORYROW_H