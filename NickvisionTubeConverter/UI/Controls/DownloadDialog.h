#pragma once

#include <QDialog>
#include "ui_DownloadDialog.h"
#include "../../Models/Download.h"

namespace NickvisionTubeConverter::UI::Controls
{
	/// <summary>
	/// A dialog for creating a Download
	/// </summary>
	class DownloadDialog : public QDialog
	{
		Q_OBJECT

	public:
		/// <summary>
		/// Constructs a DownloadDialog
		/// </summary>
		/// <param name="parent">The parent of the dialog</param>
		DownloadDialog(QWidget* parent, const QString& videoUrl = "");
		/// <summary>
		/// Gets the Download object created by the dialog
		/// </summary>
		NickvisionTubeConverter::Models::Download getDownload() const;


	private slots:
		/// <summary>
		/// Prompts the user to select a save folder
		/// </summary>
		void on_btnSelectSaveFolder_clicked();
		/// <summary>
		/// Closes the dialog sending QDialog::Accepted
		/// </summary>
		void on_btnDownload_clicked();
		/// <summary>
		/// Closes the dialog sending QDialog::Rejected
		/// </summary>
		void on_btnCancel_clicked();

	private:
		//==UI==//
		Ui::DownloadDialog m_ui;
	};
}
