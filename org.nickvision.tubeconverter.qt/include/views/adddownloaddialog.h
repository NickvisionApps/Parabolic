#ifndef ADDDOWNLOADDIALOG_H
#define ADDDOWNLOADDIALOG_H

#include <memory>
#include <QCloseEvent>
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

    protected:
        /**
         * @brief Handles when the window is closed.
         * @param event QCloseEvent
         */
        void closeEvent(QCloseEvent* event) override;

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
        void onCmbCredentialChanged(int index);
        /**
         * @brief Validates the media url.
         */
        void validateUrl();
        /**
         * @brief Handles when the file type's combobox index has changed.
         * @param index The new index
         */
        void onCmbFileTypeChanged(int index);
        /**
         * @brief Shows the generic file type disclaimer for a single download.
         */
        void genericFileTypeDisclaimerSingle();
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
         * @brief Shows the generic file type disclaimer for a playlist download.
         */
        void genericFileTypeDisclaimerPlaylist();
        /**
         * @brief Prompts the user to select a save folder for a playlist download.
         */
        void selectSaveFolderPlaylist();
        /**
         * @brief Shows the save folder disclaimer for a playlist download.
         */
        void saveFolderDisclaimerPlaylist();
        /**
         * @brief Handles when the chkNumberTitlesPlaylist's state has changed.
         * @param checked Whether or not the switch is checked
         */
        void onNumberTitlesPlaylistChanged(bool checked);
        /**
         * @brief Selects all items for a playlist download.
         */
        void selectAllPlaylist();
        /**
         * @brief Deselects all items for a playlist download.
         */
        void deselectAllPlaylist();
        /**
         * @brief Reverts the selected playlist item's filename to its original title.
         */
        void revertToTitlePlaylist();
        /**
         * @brief Downloads a playlist.
         */
        void downloadPlaylist();

    private:
        /**
         * @brief Handles when the url has been validated.
         * @param valid Whether or not the url is valid
         */
        void onUrlValidated(bool valid);
        Ui::AddDownloadDialog* m_ui;
        std::shared_ptr<Shared::Controllers::AddDownloadDialogController> m_controller;
    };
}

#endif //ADDDOWNLOADDIALOG_H
