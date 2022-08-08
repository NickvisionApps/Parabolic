#pragma once

#include <QUrl>
#include <QWidget>
#include "ui_BrowsePage.h"

namespace NickvisionTubeConverter::UI::Views
{
	class BrowsePage : public QWidget
	{
		Q_OBJECT

	public:
		BrowsePage(QWidget* parent = nullptr);
		/// <summary>
		/// Refreshes the theme of the page
		/// </summary>
		void refreshTheme();

	private slots:
		void on_btnBack_clicked();
		void on_btnForward_clicked();
		void on_btnRefresh_clicked();
		void on_btnHome_clicked();
		void on_btnDownload_clicked();
		void on_webView_urlChanged(const QUrl& url);

	private:
		//==UI==//
		Ui::BrowsePage m_ui;
	};

}