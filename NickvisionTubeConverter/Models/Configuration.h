#pragma once

#include <string>
#include "Theme.h"

namespace NickvisionTubeConverter::Models
{
	/// <summary>
	/// A model for an application's configuration
	/// </summary>
	class Configuration
	{
	public:
		Configuration(const Configuration&) = delete;
		void operator=(const Configuration&) = delete;
		/// <summary>
		/// Gets the Configuration singleton object
		/// </summary>
		/// <returns>A reference to the AppInfo object</returns>
		static Configuration& getInstance();
		/// <summary>
		/// Gets the theme of the application
		/// </summary>
		/// <param name="calculateSystemTheme">Determines if Theme::System should be calculated into Theme::Light or Theme::Dark depending on system configuration. Set true to determine or false to leave as Theme::System</param>
		/// <returns>The theme of the application</returns>
		Theme getTheme(bool calculateSystemTheme = true) const;
		/// <summary>
		/// Sets the theme of the application
		/// </summary>
		/// <param name="theme">The theme of the application</param>
		void setTheme(Theme theme);
		/// <summary>
		/// Gets whether or not to start on home page
		/// </summary>
		/// <returns>True to start on home page, else false</returns>
		bool getAlwaysStartOnHomePage() const;
		/// <summary>
		/// Sets whether or not to start on home page
		/// </summary>
		/// <param name="alwaysStartOnHomePage">True for yes, false for no</param>
		void setAlwaysStartOnHomePage(bool alwaysStartOnHomePage);
		/// <summary>
		/// Saves the configuration file to disk
		/// </summary>
		void save() const;

	private:
		/// <summary>
		/// Constructs a Configuration object
		/// </summary>
		Configuration();
		std::string m_configDir;
		Theme m_theme;
		bool m_alwaysStartOnHomePage;
	};
}

