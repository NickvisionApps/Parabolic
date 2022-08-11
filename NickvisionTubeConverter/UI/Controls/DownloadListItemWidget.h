#pragma once

#include <mutex>
#include <thread>
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
		/// Updates the UI from the timer
		/// </summary>
		void timeout();

	private:
		//==Vars==//
		std::mutex m_mutex;
		NickvisionTubeConverter::Models::Download m_download;
		bool m_isFinished;
		bool m_isSuccess;
		std::jthread m_thread;
		//==UI==//
		Ui::DownloadListItemWidget m_ui;
		QTimer* m_timer{ nullptr };
	};

}