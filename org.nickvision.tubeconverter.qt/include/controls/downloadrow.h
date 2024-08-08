#ifndef DOWNLOADROW_H
#define DOWNLOADROW_H

#include <filesystem>
#include <QString>
#include <QWidget>
#include "events/downloadaddedeventargs.h"
#include "events/downloadcompletedeventargs.h"
#include "events/downloadprogresschangedeventargs.h"

namespace Ui { class DownloadRow; }

namespace Nickvision::TubeConverter::QT::Controls
{
    /**
     * @brief A row that displays and manages a download.
     */
    class DownloadRow : public QWidget
    {
    Q_OBJECT

    public:
        /**
         * @brief Constructs a DownloadRow.
         * @param args DownloadAddedEventArgs
         * @param parent The parent widget
         */
        DownloadRow(const Shared::Events::DownloadAddedEventArgs& args, QWidget* parent = nullptr);
        /**
         * @brief Destructs a DownloadRow.
         */
        ~DownloadRow();
        /**
         * @brief Gets the id of the download.
         * @return The id of the download
         */
        int getId() const;
        /**
         * @brief Gets the log of the download.
         * @return The log of the download
         */
        const QString& getLog() const;
        /**
         * @brief Updates the row with the new download progress.
         * @param args DownloadProgressChangedEventArgs
         */
        void setProgressState(const Shared::Events::DownloadProgressChangedEventArgs& args);
        /**
         * @brief Updates the row with the completed download state.
         * @param args DownloadCompletedEventArgs
         */
        void setCompleteState(const Shared::Events::DownloadCompletedEventArgs& args);
        /**
         * @brief Updates the row with the stopped download state.
         */
        void setStopState();
        /**
         * @brief Updates the row with the started from queue state.
         */
        void setStartFromQueueState();

    Q_SIGNALS:
        /**
         * @brief Emitted when the stop button is clicked.
         * @param id The id of the download
         */
        void stop(int id);
        /**
         * @brief Emitted when the retry button is clicked.
         * @param id The id of the download
         */
        void retry(int id);

    private Q_SLOTS:
        /**
         * @brief Opens the download in a media player.
         */
        void play();
        /**
         * @brief Opens the download folder.
         */
        void openFolder();

    private:
        Ui::DownloadRow* m_ui;
        int m_id;
        QString m_log;
        std::filesystem::path m_path;
    };
}

#endif //DOWNLOADROW_H