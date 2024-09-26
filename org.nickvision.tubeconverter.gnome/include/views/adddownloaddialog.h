#ifndef ADDDOWNLOADDIALOG_H
#define ADDDOWNLOADDIALOG_H

#include <memory>
#include <vector>
#include <adwaita.h>
#include "controllers/adddownloaddialogcontroller.h"
#include "helpers/dialogbase.h"

namespace Nickvision::TubeConverter::GNOME::Views
{
    /**
     * @brief The add download dialog for the application.
     */
    class AddDownloadDialog : public Helpers::DialogBase
    {
    public:
        /**
         * @brief Constructs a AddDownloadDialog.
         * @param controller The AddDownloadDialogController
         * @param parent The GtkWindow object of the parent window
         * @param url An optional url to start download validation with
         */
        AddDownloadDialog(const std::shared_ptr<Shared::Controllers::AddDownloadDialogController>& controller, const std::string& url, GtkWindow* parent);

    private:
        /**
         * @brief Handles when the url text is changed.
         */
        void onTxtUrlChanged();
        /**
         * @brief Handles when the credential combobox is changed.
         */
        void onCmbCredentialChanged();
        /**
         * @brief Validates the url.
         */
        void validateUrl();
        /**
         * @brief Handles when a url is validated.
         */
        void onUrlValidated();
        /**
         * @brief Goes back to the single download page.
         */
        void back();
        /**
         * @brief Handles when the single quality comboboxs are changed.
         */
        void onQualitySingleChanged();
        /**
         * @brief Shows the subtitles page for a single download.
         */
        void subtitlesSingle();
        /**
         * @brief Shows the advanced options page for a single download.
         */
        void advancedOptionsSingle();
        /**
         * @brief Prompts the user to select a folder to save a single download.
         */
        void selectSaveFolderSingle();
        /**
         * @brief Reverts the filename for a single download to its original title.
         */
        void revertFilenameSingle();
        /**
         * @brief Reverts the start time for a single download to its original value.
         */
        void revertStartTimeSingle();
        /**
         * @brief Reverts the end time for a single download to its original value.
         */
        void revertEndTimeSingle();
        /**
         * @brief Adds the single download to the download queue.
         */
        void downloadSingle();
        /**
         * @brief Prompts the user to select a folder to save a playlist download.
         */
        void selectSaveFolderPlaylist();
        /**
         * @brief Shows the items for a playlist download.
         */
        void itemsPlaylist();
        /**
         * @brief Handles when the number titles switch is changed.
         */
        void onNumberTitlesPlaylistChanged();
        /**
         * @brief Adds the playlist downloads to the download queue.
         */
        void downloadPlaylist();
        std::shared_ptr<Shared::Controllers::AddDownloadDialogController> m_controller;
        std::vector<AdwActionRow*> m_singleSubtitleRows;
        std::vector<GtkCheckButton*> m_singleSubtitleCheckButtons;
        std::vector<AdwEntryRow*> m_playlistItemRows;
        std::vector<GtkCheckButton*> m_playlistItemCheckButtons;
    };
}

#endif //ADDDOWNLOADDIALOG_H