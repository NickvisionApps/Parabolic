#ifndef ADDDOWNLOADDIALOG_H
#define ADDDOWNLOADDIALOG_H

#include <memory>
#include <QDialog>
#include "controllers/adddownloaddialogcontroller.h"

namespace Ui { class AddDownloadDialog; }

namespace Nickvision::TubeConverter::Qt::Views
{
    /**
     * @brief The add download dialog for the application.
     */
    class AddDownloadDialog : public QDialog
    {
    Q_OBJECT

    public:
        /**
         * @brief Constructs an AddDownloadDialog.
         * @param controller The AddDownloadDialogController
         * @param url An optional url to start download validation with
         * @param parent The parent widget
         */
        AddDownloadDialog(const std::shared_ptr<Shared::Controllers::AddDownloadDialogController>& controller, const std::string& url, QWidget* parent = nullptr);
        /**
         * @brief Destructs an AddDownloadDialog.
         */
        ~AddDownloadDialog();

    private Q_SLOTS:
        /**
         * @brief Handles when the txtUrl's text has changed.
         * @param text The new text
         */
        void onTxtUrlChanged(const QString& text);
        /**
         * @brief Prompts the user to select a batch file to use instead of a url.
         */
        void useBatchFile();
        /**
         * @brief Handles when the cmbAuthenticate's index has changed.
         * @param index The new index
         */
        void onCmbAuthenticateChanged(int index);
        /**
         * @brief Validates the media url.
         */
        void validateUrl();
        /**
         * @brief Handles when the cmbFileTypeSingle's index has changed.
         * @param index The new index
         */
        void onCmbFileTypeSingleChanged(int index);
        /**
         * @brief Prompts the user to select a save folder for a single download.
         */
        void selectSaveFolderSingle();
        /**
         * @brief Reverts the filename of a single download to its original title.
         */
        void revertFilenameSingle();
        /**
         * @brief Selects all subtitles for a single download.
         */
        void selectAllSubtitlesSingle();
        /**
         * @brief Deselects all subtitles for a single download.
         */
        void deselectAllSubtitlesSingle();
        /**
         * @brief Downloads a single media.
         */
        void downloadSingle();
        /**
         * @brief Handles when the cmbFileTypePlaylist's index has changed.
         * @param index The new index
         */
        void onCmbFileTypePlaylistChanged(int index);
        /**
         * @brief Prompts the user to select a save folder for a playlist download.
         */
        void selectSaveFolderPlaylist();
        /**
         * @brief Handles when the chkNumberTitlesPlaylist's state has changed.
         * @param state The new state
         */
        void onNumberTitlesPlaylistChanged(int state);
        /**
         * @brief Selects all items for a playlist download.
         */
        void selectAllPlaylist();
        /**
         * @brief Deselects all items for a playlist download.
         */
        void deselectAllPlaylist();
        /**
         * @brief Downloads a playlist.
         */
        void downloadPlaylist();

    private:
        /**
         * @brief Handles when the url has been validated.
         */
        void onUrlValidated();
        Ui::AddDownloadDialog* m_ui;
        std::shared_ptr<Shared::Controllers::AddDownloadDialogController> m_controller;
    };
}

#endif //ADDDOWNLOADDIALOG_H
