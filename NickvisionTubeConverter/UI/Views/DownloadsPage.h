#pragma once

#include <QWidget>
#include "ui_DownloadsPage.h"
#include "../../Models/Download.h"

namespace NickvisionTubeConverter::UI::Views
{
	/// <summary>
	/// The downloads page for the application
	/// </summary>
	class DownloadsPage : public QWidget
	{
		Q_OBJECT

	public:
		/// <summary>
		/// Constructs a DownloadsPage
		/// </summary>
		/// <param name="parent">The parent of the widget, if any</param>
		DownloadsPage(QWidget* parent = nullptr);
		/// <summary>
		/// Refreshes the theme of the page
		/// </summary>
		void refreshTheme();

	private slots:
		/// <summary>
		/// Prompts the user to add a download with DownloadDialog
		/// </summary>
		void on_btnAddDownload_clicked();
		/// <summary>
		/// Removes the completed downloads from the downloads list
		/// </summary>
		void on_btnClearCompletedDownloads_clicked();

	private:
		//==UI==//
		Ui::DownloadsPage m_ui;
		//==Functions==//
		void addDownload(const NickvisionTubeConverter::Models::Download& download);
	};
}
