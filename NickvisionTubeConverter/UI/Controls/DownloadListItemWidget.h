#pragma once

#include <mutex>
#include <thread>
#include <QMouseEvent>
#include <QTimer>
#include <QWidget>
#include "ui_DownloadListItemWidget.h"
#include "../../Models/Download.h"

namespace NickvisionTubeConverter::UI::Controls
{
	/// <summary>
	/// A widget for a QListWidgetItem representing a download
	/// </summary>
	class DownloadListItemWidget : public QWidget
	{
		Q_OBJECT

	public:
		/// <summary>
		/// Constructs a DownloadListItemWidget
		/// </summary>
		/// <param name="download">The Download to run</param>
		/// <param name="parent">The parent of the widget, if any</param>
		DownloadListItemWidget(const NickvisionTubeConverter::Models::Download& download, QWidget* parent = nullptr);
		/// <summary>
		/// Gets whether or not the download is finished
		/// </summary>
		/// <returns>True for finished, else false</returns>
		bool getIsFinished() const;
		/// <summary>
		/// Updates the UI from the timer
		/// </summary>
		void timeout();

	protected:
		void mouseDoubleClickEvent(QMouseEvent* event) override;

	private slots:
		/// <summary>
		/// Shows a dialog with the log of the download
		/// </summary>
		void on_btnLog_clicked();

	private:
		//==Vars==//
		mutable std::mutex m_mutex;
		NickvisionTubeConverter::Models::Download m_download;
		bool m_isFinished;
		bool m_isSuccess;
		std::jthread m_thread;
		//==UI==//
		Ui::DownloadListItemWidget m_ui;
		QTimer* m_timer{ nullptr };
	};

}