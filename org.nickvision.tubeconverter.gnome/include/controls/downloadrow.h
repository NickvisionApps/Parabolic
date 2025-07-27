#ifndef DOWNLOADROW_H
#define DOWNLOADROW_H

#include <filesystem>
#include <string>
#include <adwaita.h>
#include <libnick/events/event.h>
#include <libnick/events/parameventargs.h>
#include "events/downloadaddedeventargs.h"
#include "events/downloadcompletedeventargs.h"
#include "events/downloadprogresschangedeventargs.h"
#include "helpers/controlbase.h"

namespace Nickvision::TubeConverter::GNOME::Controls
{
    /**
     * @brief A row that displays and manages a download.
     */
    class DownloadRow : public Helpers::ControlBase<GtkListBoxRow>
    {
    public:
        /**
         * @brief Constructs a DownloadRow.
         * @param args DownloadAddedEventArgs
         * @param parent The GtkWindow object of the parent window
         */
        DownloadRow(const Shared::Events::DownloadAddedEventArgs& args, GtkWindow* parent);
        /**
         * @brief Gets the id of the download.
         * @return The id of the download
         */
        int getId();
        /**
         * @brief Gets the log of the download.
         * @return The log of the download
         */
        const std::string& getLog();
        /**
         * @brief Gets the event for when the download is stopped.
         * @return The stopped event
         */
        Events::Event<Events::ParamEventArgs<int>>& stopped();
        /**
         * @brief Gets the event for when the download is paused.
         * @return The paused event
         */
        Events::Event<Events::ParamEventArgs<int>>& paused();
        /**
         * @brief Gets the event for when the download is resumed.
         * @return The resumed event
         */
        Events::Event<Events::ParamEventArgs<int>>& resumed();
        /**
         * @brief Gets the event for when the download is retried.
         * @return The retried event
         */
        Events::Event<Events::ParamEventArgs<int>>& retried();
        /**
         * @brief Gets the event for when the request is made to copy the download command to the clipboard.
         * @return The command to clipboard request event
         */
        Events::Event<Events::ParamEventArgs<int>>& commandToClipboardRequested();
        /**
         * @brief Updates the row with the new download progress.
         * @param args DownloadProgressChangedEventArgs
         */
        void setProgressState(const Shared::Events::DownloadProgressChangedEventArgs& args);
        /**
         * @brief Updates the row with the completed download state.
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

    private:
        /**
         * @brief Pauses or resumes the download.
         */
        void pauseResume();
        /**
         * @brief Stops the download.
         */
        void stop();
        /**
         * @brief Plays the download.
         */
        void play();
        /**
         * @brief Opens the download folder.
         */
        void openFolder();
        /**
         * @brief Retries the download.
         */
        void retry();
        /**
         * @brief Copies the download command to the clipboard.
         */
        void cmdToClipboard();
        /**
         * @brief Copies the download log to the clipboard.
         */
        void logToClipboard();
        int m_id;
        std::string m_log;
        std::filesystem::path m_path;
        bool m_isPaused;
        Events::Event<Events::ParamEventArgs<int>> m_stopped;
        Events::Event<Events::ParamEventArgs<int>> m_paused;
        Events::Event<Events::ParamEventArgs<int>> m_resumed;
        Events::Event<Events::ParamEventArgs<int>> m_retried;
        Events::Event<Events::ParamEventArgs<int>> m_commandToClipboardRequested;
    };
}

#endif //DOWNLOADROW_H
