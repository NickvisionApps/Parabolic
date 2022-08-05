#pragma once

#include <QDialog>
#include "ui_AboutDialog.h"
#include "../../Models/AppInfo.h"

namespace NickvisionTubeConverter::UI::Controls
{
	/// <summary>
	/// A dialog for displaying application information
	/// </summary>
	class AboutDialog : public QDialog
	{
		Q_OBJECT

	public:
		/// <summary>
		/// Constructs an AboutDialog
		/// </summary>
		/// <param name="parent">The parent of the dialog</param>
		AboutDialog(QWidget* parent);

	private slots:
		/// <summary>
		/// Opens the default browser to the application's GitHub repo
		/// </summary>
		void on_btnGitHubRepo_clicked();
		/// <summary>
		/// Opens the default browser to the application's issue tracker
		/// </summary>
		void on_btnReportABug_clicked();
		/// <summary>
		/// Closes the dialog
		/// </summary>
		void on_btnClose_clicked();

	private:
		//==Vars==//
		NickvisionTubeConverter::Models::AppInfo& m_appInfo;
		//==UI==//
		Ui::AboutDialog m_ui;
	};
}
