#pragma once

#include <QPalette>
#include <QString>
#include <QWidget>

/// <summary>
/// Functions for working with an application's theme
/// </summary>
namespace NickvisionTubeConverter::Helpers::ThemeHelpers
{
	/// <summary>
	/// Gets a themed QPalette
	/// </summary>
	/// <returns>QPalette themed for Configuration::getTheme()</returns>
	QPalette getThemedPalette();
	/// <summary>
	/// Gets a themed stylesheet for a separator 
	/// </summary>
	/// <returns>Separator stylesheet themed for Configuration::getTheme()</returns>
	QString getThemedSeparatorStyle();
	/// <summary>
	/// Applys Win32 theming to QWidget's Title Bar
	/// </summary>
	/// <param name="widget">The QWidget object</param>
	void applyWin32Theme(QWidget* widget);
}

