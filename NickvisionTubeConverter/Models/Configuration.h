#pragma once

#include <string>
#include "Theme.h"
#include "MediaFileType.h"
#include "../Update/Version.h"

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
		/// Gets the version of ytdlp downloaded
		/// </summary>
		/// <returns>The version of ytdlp downloaded</returns>
		const NickvisionTubeConverter::Update::Version& getYtdlpVersion() const;
		/// <summary>
		/// Sets the version of ytdlp downloaded
		/// </summary>
		/// <param name="ytdlpVersion">The new version of ytdlp</param>
		void setYtdlpVersion(const NickvisionTubeConverter::Update::Version& ytdlpVersion);
		/// <summary>
		/// Gets the version of ffmpeg downloaded
		/// </summary>
		/// <returns>The version of ffmpeg downloaded</returns>
		const NickvisionTubeConverter::Update::Version& getFfmpegVersion() const;
		/// <summary>
		/// Sets the version of ffmpeg downloaded
		/// </summary>
		/// <param name="ffmpegVersion">The new version of ffmpeg</param>
		void setFfmpegVersion(const NickvisionTubeConverter::Update::Version& ffmpegVersion);
		/// <summary>
		/// Gets the previous save folder used in a download
		/// </summary>
		/// <returns>The previous save folder used in a download</returns>
		const std::string& getPreviousSaveFolder() const;
		/// <summary>
		/// Sets the previous save folder used in a download
		/// </summary>
		/// <param name="previousSaveFolder">The new previous save folder</param>
		void setPreviousSaveFolder(const std::string& previousSaveFolder);
		/// <summary>
		/// Gets the previous file format used in a download
		/// </summary>
		/// <returns>The previous file format used in a download</returns>
		const MediaFileType& getPreviousFileFormat() const;
		/// <summary>
		/// Sets the previous file format used in a download
		/// </summary>
		/// <param name="previousFileFormat">The new previous file format</param>
		void setPreviousFileForamt(const MediaFileType& previousFileFormat);
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
		NickvisionTubeConverter::Update::Version m_ytdlpVersion;
		NickvisionTubeConverter::Update::Version m_ffmpegVersion;
		std::string m_previousSaveFolder;
		MediaFileType m_previousFileFormat;
	};
}

