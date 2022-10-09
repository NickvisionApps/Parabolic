#pragma once

#include <adwaita.h>
#include "../../models/download.hpp"

namespace NickvisionTubeConverter::UI::Controls
{
	/**
	 * A widget for managing a download
	 */
	class DownloadRow
	{
	public:
		/**
		 * Constructs a DownloadRow
		 *
		 * @param parent The parent window the holds the row
		 * @param download The Download model to manage
		 */
		DownloadRow(GtkWindow* parent, const NickvisionTubeConverter::Models::Download& download);
		/**
		 * Gets the GtkWidget* representing the DownloadRow
		 *
		 * @returns The GtkWidget* representing the DownloadRow
		 */
		GtkWidget* gobj();
		/**
		 * Starts the Download managed by the row
		 */
		void start();

	private:
		NickvisionTubeConverter::Models::Download m_download;
		GtkWidget* m_gobj;
		GtkWindow* m_parent;
		GtkWidget* m_imgStatus;
		GtkWidget* m_viewStack;
		GtkWidget* m_boxDownloading;
		GtkWidget* m_progBar;
		GtkWidget* m_boxDone;
		GtkWidget* m_levelBar;
		GtkWidget* m_btnViewLogs;
		/**
		 * Displays a MessageDialog with the log from the download
		 */
		void onViewLogs();
	};
}