#pragma once

#include <adwaita.h>
#include "../../controllers/mainwindowcontroller.hpp"

namespace NickvisionTubeConverter::UI::Views
{
	/**
	 * The MainWindow for the application
	 */
	class MainWindow
	{
	public:
		/**
		 * Constructs a MainWindow
		 *
		 * @param application GtkApplication*
		 * @param appInfo The AppInfo for the application
		 */
		MainWindow(GtkApplication* application, const NickvisionTubeConverter::Controllers::MainWindowController& controller);
		/**
		 * Gets the GtkWidget* representing the MainWindow
		 *
		 * @returns The GtkWidget* representing the MainWindow
		 */
		GtkWidget* gobj();
		/**
		 * Starts the MainWindow
		 */
		void start();

	private:
		NickvisionTubeConverter::Controllers::MainWindowController m_controller;
		GtkWidget* m_gobj{ nullptr };
		GtkWidget* m_mainBox{ nullptr };
		GtkWidget* m_headerBar{ nullptr };
		GtkWidget* m_adwTitle{ nullptr };
		GtkWidget* m_btnAddDownload{ nullptr };
		GtkWidget* m_btnMenuHelp{ nullptr };
		GtkWidget* m_toastOverlay{ nullptr };
		GtkWidget* m_viewStack{ nullptr };
		GtkWidget* m_pageStatusNoDownloads{ nullptr };
		GtkWidget* m_pageScrollDownloads{ nullptr };
		GtkWidget* m_listDownloads{ nullptr };
		GSimpleAction* m_actAddDownload{ nullptr };
		GSimpleAction* m_actPreferences{ nullptr };
		GSimpleAction* m_actKeyboardShortcuts{ nullptr };
		GSimpleAction* m_actAbout{ nullptr };
		/**
		 * Prompts the user to add a download with AddDownloadDialog
		 */
		void onAddDownload();
		/**
		 * Displays the preferences dialog
		 */
		void onPreferences();
		/**
		 * Displays the keyboard shortcuts dialog
		 */
		void onKeyboardShortcuts();
		/**
		 * Displays the about dialog
		 */
		void onAbout();
	};
}