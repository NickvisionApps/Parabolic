#pragma once

#include <QUrl>
#include <QWidget>
#include "ui_BrowsePage.h"

namespace NickvisionTubeConverter::UI::Views
{
	/// <summary>
	/// The browse page for the application
	/// </summary>
	class BrowsePage : public QWidget
	{
		Q_OBJECT

	public:
		/// <summary>
		/// Constructs a BrowsePage
		/// </summary>
		/// <param name="parent">The parent of the widget, if any</param>
		BrowsePage(QWidget* parent = nullptr);
		/// <summary>
		/// Refreshes the theme of the page
		/// </summary>
		void refreshTheme();

	private slots:
		/// <summary>
		/// Navigates the webView back
		/// </summary>
		void on_btnBack_clicked();
		/// <summary>
		/// Navigates the webView forward
		/// </summary>
		void on_btnForward_clicked();
		/// <summary>
		/// Refreshes the webView page
		/// </summary>
		void on_btnRefresh_clicked();
		/// <summary>
		/// Navigates the webView home
		/// </summary>
		void on_btnHome_clicked();
		/// <summary>
		/// Displays the DownloadDialog for adding the download to the queue
		/// </summary>
		void on_btnDownload_clicked();
		/// <summary>
		/// Updates the UI when the webView url is changed
		/// </summary>
		/// <param name="url">The new url</param>
		void on_webView_urlChanged(const QUrl& url);

	private:
		//==UI==//
		Ui::BrowsePage m_ui;
	};

}