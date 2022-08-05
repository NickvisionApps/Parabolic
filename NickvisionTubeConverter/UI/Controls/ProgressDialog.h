#pragma once

#include <functional>
#include <QDialog>
#include <QString>
#include "ui_ProgressDialog.h"

namespace NickvisionTubeConverter::UI::Controls
{
	/// <summary>
	/// A dialog for managing progress and running long tasks
	/// </summary>
	class ProgressDialog : public QDialog
	{
		Q_OBJECT

	public:
		/// <summary>
		/// Constructs a ProgressDialog
		/// </summary>
		/// <param name="parent">The parent of the dialog</param>
		/// <param name="description">The description of the long task</param>
		/// <param name="work">The long task</param>
		ProgressDialog(QWidget* parent, const QString& description, const std::function<void()>& work);
		/// <summary>
		/// Runs the dialog and waits for the long task to be finished, while displaying progress
		/// </summary>
		/// <returns>The exit code of the dialog</returns>
		int exec();

	private:
		//==Vars==//
		bool m_isFinished;
		std::function<void()> m_work;
		//==UI==//
		Ui::ProgressDialog m_ui;
	};
}
