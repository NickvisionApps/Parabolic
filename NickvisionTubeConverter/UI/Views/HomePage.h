#pragma once

#include <QWidget>
#include "ui_HomePage.h"

namespace NickvisionTubeConverter::UI::Views
{
	/// <summary>
	/// A home page for the application
	/// </summary>
	class HomePage : public QWidget
	{
		Q_OBJECT

	public:
		/// <summary>
		/// Constructs a HomePage
		/// </summary>
		/// <param name="parent">The parent of the widget, if any</param>
		HomePage(QWidget* parent = nullptr);
		/// <summary>
		/// Refreshes the theme of the page
		/// </summary>
		void refreshTheme();

	private slots:
		/// <summary>
		/// Updates the alwaysStartOnHomePage configuration preference
		/// </summary>
		void on_chkAlwaysStartOnHomePage_clicked();

	private:
		//==UI==//
		Ui::HomePage m_ui;
	};
}
