#pragma once

#include <string>
#include "mediafiletype.hpp"

namespace NickvisionTubeConverter::Models
{
	/**
	 * Themes for the application
	 */
	enum class Theme
	{
		System = 0,
		Light,
		Dark
	};

	/**
	 * A model for the settings of the application
	 */
	class Configuration
	{
	public:
		/**
		 * Constructs a Configuration (loading the configuraton from disk)
		 */
		Configuration();
		/**
		 * Gets the requested theme
		 *
		 * @returns The requested theme
		 */
		Theme getTheme() const;
		/**
		 * Sets the requested theme
		 *
		 * @param theme The new theme
		 */
		void setTheme(Theme theme);
		/**
		 * Gets the previous save folder used in a download
		 *
		 * @returns The previous save folder
		 */
		const std::string& getPreviousSaveFolder() const;
		/**
		 * Sets the previous save folder used in a download
		 *
		 * @param previousSaveFolder The new previous save folder
		 */
		void setPreviousSaveFolder(const std::string& previousSaveFolder);
		/**
		 * Gets the previous file format used in a download
		 *
		 * @returns The previous file format
		 */
		const MediaFileType& getPreviousFileFormat() const;
		/**
		 * Sets the previous file format used in a download
		 *
		 * @param previousFileFormat The new previous file format
		 */
		void setPreviousFileForamt(const MediaFileType& previousFileFormat);
		/**
		 * Saves the configuration to disk
		 */
		void save() const;

	private:
		std::string m_configDir;
		Theme m_theme;
		std::string m_previousSaveFolder;
		MediaFileType m_previousFileFormat;
	};
}