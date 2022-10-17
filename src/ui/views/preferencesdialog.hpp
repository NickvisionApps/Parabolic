#pragma once

#include <adwaita.h>
#include "../../controllers/preferencesdialogcontroller.hpp"

namespace NickvisionTubeConverter::UI::Views
{
	/**
	 * A dialog for managing appplication preferences
	 */
	class PreferencesDialog
	{
	public:
		/**
		 * Constructs a PreferencesDialog
		 *
		 * @param parent The parent window for the dialog
		 * @param controller PreferencesDialogController
		 */
		PreferencesDialog(GtkWindow* parent, const NickvisionTubeConverter::Controllers::PreferencesDialogController& controller);
		/**
		 * Gets the GtkWidget* representing the PreferencesDialog
		 *
		 * @returns The GtkWidget* representing the PreferencesDialog
		 */
		GtkWidget* gobj();
		/**
		 * Runs the PreferencesDialog
		 */
		void run();

	private:
		NickvisionTubeConverter::Controllers::PreferencesDialogController m_controller;
		GtkWidget* m_gobj{ nullptr };
		GtkWidget* m_mainBox{ nullptr };
		GtkWidget* m_headerBar{ nullptr };
		GtkWidget* m_page{ nullptr };
		GtkWidget* m_grpUserInterface{ nullptr };
		GtkWidget* m_rowTheme{ nullptr };
		GtkWidget* m_grpConverter{ nullptr };
		GtkWidget* m_rowEmbedMetadata{ nullptr };
		GtkWidget* m_switchEmbedMetadata{ nullptr };
	};
}