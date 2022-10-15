#pragma once

#include <memory>
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
		DownloadRow(GtkWindow* parent, const std::shared_ptr<NickvisionTubeConverter::Models::Download>& download);
		/**
		 * Gets the GtkWidget* representing the DownloadRow
		 *
		 * @returns The GtkWidget* representing the DownloadRow
		 */
		GtkWidget* gobj();
		/**
		 * Gets whether or not the Download is completed (successful or not)
		 *
		 * @returns True if completed, else false
		 */
		bool getIsDone() const;
		/**
		 * Starts the Download managed by the row
		 *
		 * @param iterateMainContext True to run g_main_context_iteration, else false. (Only 1 running row should be true)
		 */
		void start(bool iterateMainContext = false);
		/**
		 * Stops the download
		 */
		void stop();

	private:
		std::shared_ptr<NickvisionTubeConverter::Models::Download> m_download;
		bool m_isDone;
		GtkWidget* m_gobj;
		GtkWindow* m_parent;
		GtkWidget* m_imgStatus;
		GtkWidget* m_viewStack;
		GtkWidget* m_boxDownloading;
		GtkWidget* m_progBar;
		GtkWidget* m_btnStop;
		GtkWidget* m_boxDone;
		GtkWidget* m_levelBar;
		GtkWidget* m_btnViewLogs;
		/**
		 * Displays a MessageDialog with the log from the download
		 */
		void onViewLogs();
	};
}