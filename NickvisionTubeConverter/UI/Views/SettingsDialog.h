#pragma once

#include <QDialog>
#include "ui_SettingsDialog.h"
#include "../../Models/Configuration.h"

namespace NickvisionTubeConverter::UI::Views
{
	/// <summary>
	/// A dialog for managing application settings
	/// </summary>
	class SettingsDialog : public QDialog
	{
		Q_OBJECT

	public:
		/// <summary>
		/// Constructs a SettingsDialog
		/// </summary>
		/// <param name="parent">The parent of the dialog</param>
		SettingsDialog(QWidget* parent);

	private slots:
		/// <summary>
		/// Navigate to user interface view
		/// </summary>
		void on_navUserInterface_clicked();
		/// <summary>
		/// Navigate to application view
		/// </summary>
		void on_navApplication_clicked();
		/// <summary>
		/// Saves the configuration and closes the dialog
		/// </summary>
		void on_btnSave_clicked();
		/// <summary>
		/// Discards the changes to the configuration and closes the dialog
		/// </summary>
		void on_btnCancel_clicked();

	private:
		/// <summary>
		/// SettingsDialog views
		/// </summary>
		enum class Views
		{
			UserInterface = 0,
			Application
		};
		//==Vars==//
		NickvisionTubeConverter::Models::Configuration& m_configuration;
		//==UI==//
		Ui::SettingsDialog m_ui;
		//==Functions==//
		/// <summary>
		/// Changes the view on the dialog
		/// </summary>
		/// <param name="view">The view to change to</param>
		void changeView(Views view);
	};
}
