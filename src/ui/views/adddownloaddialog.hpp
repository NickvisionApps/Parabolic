#pragma once

#include <string>
#include <adwaita.h>
#include "../../controllers/adddownloaddialogcontroller.hpp"

namespace NickvisionTubeConverter::UI::Views
{
	/**
     * A dialog for creating a download
     */
	class AddDownloadDialog
	{
	public:
		/**
		 * Constructs an AddDownloadDialog
		 *
		 * @param parent The parent window for the dialog
		 * @param controller The AddDownloadDialogController
		 */
		AddDownloadDialog(GtkWindow* parent, NickvisionTubeConverter::Controllers::AddDownloadDialogController& controller);
		/**
    	 * Gets the GtkWidget* representing the AddDownloadDialog
    	 *
    	 * @returns The GtkWidget* representing the AddDownloadDialog
    	 */
    	GtkWidget* gobj();
    	/**
    	 * Run the AddDownloadDialog
    	 *
    	 * @returns The download from the dialog is one is available, else std::nullopt
    	 */
    	bool run();

	private:
		NickvisionTubeConverter::Controllers::AddDownloadDialogController& m_controller;
		std::string m_response;
		GtkWindow* m_parent{ nullptr };
    	GtkWidget* m_gobj{ nullptr };
    	GtkWidget* m_preferencesGroup{ nullptr };
    	GtkWidget* m_rowVideoUrl{ nullptr };
    	GtkWidget* m_rowFileType{ nullptr };
    	GtkWidget* m_rowQuality{ nullptr };
    	GtkWidget* m_btnSelectSaveFolder{ nullptr };
    	GtkWidget* m_rowSaveFolder{ nullptr };
    	GtkWidget* m_rowNewFilename{ nullptr };
    	/**
    	 * Sets the response
    	 *
    	 * @param The new response
    	 */
		void setResponse(const std::string& response);
		/**
		 * Prompts the user to select a save folder
		 */
		void onSelectSaveFolder();
	};
}