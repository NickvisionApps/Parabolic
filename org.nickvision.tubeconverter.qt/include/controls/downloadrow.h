#ifndef DOWNLOADROW_H
#define DOWNLOADROW_H

#include <filesystem>
#include <QString>
#include <QWidget>
#include "events/downloadaddedeventargs.h"
#include "events/downloadcompletedeventargs.h"
#include "events/downloadprogresschangedeventargs.h"

namespace Ui { class DownloadRow; }

namespace Nickvision::TubeConverter::Qt::Controls
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
         * @brief Constructs a DownloadRow.
         * @param row A DownloadRow to copy from
         */
        DownloadRow(const DownloadRow& row);
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
         * @brief Updates the row with the paused download state.
         */
        void setPauseState();
        /**
         * @brief Updates the row with the resumed download state.
         */
        void setResumeState();
        /**
         * @brief Updates the row with the started from queue state.
         */
        void setStartFromQueueState();

    Q_SIGNALS:
        /**
         * @brief Emitted when the show log button is clicked.
         * @param id The id of the download
         */
        void showLog(int id);
        /**
         * @brief Emitted when the stop button is clicked.
         * @param id The id of the download
         */
        void stop(int id);
        /**
         * @brief Emitted when the pause button is clicked.
         * @param id The id of the download
         */
        void pause(int id);
        /**
         * @brief Emitted when the resume button is clicked.
         * @param id The id of the download
         */
        void resume(int id);
        /**
         * @brief Emitted when the retry button is clicked.
         * @param id The id of the download
         */
        void retry(int id);

    private Q_SLOTS:
        /**
         * @brief Pauses or resumes the download.
         */
        void pauseResume();
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
        std::filesystem::path m_path;
        bool m_isPaused;
    };
}

#endif //DOWNLOADROW_H
