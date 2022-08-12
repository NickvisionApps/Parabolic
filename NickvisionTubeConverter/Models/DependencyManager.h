#pragma once

#include <string>
#include "../Update/Version.h"

namespace NickvisionTubeConverter::Models
{
	/// <summary>
	/// A model for managing application dependencies
	/// </summary>
	class DependencyManager
	{
	public:
		/// <summary>
		/// Constructs a DependencyManager
		/// </summary>
		DependencyManager();
		/// <summary>
		/// Gets whether or not the yt-dlp dependency exists
		/// </summary>
		/// <returns>True if exists, else false</returns>
		bool getYtdlpExists() const;
		/// <summary>
		/// Gets whether or not the ffmpeg dependency exists
		/// </summary>
		/// <returns>True if exists, else false</returns>
		bool getFfmpegExists() const;
		/// <summary>
		/// Downloads the non-existing dependencies
		/// </summary>
		/// <returns>True if all dependencies are downloaded, else false</returns>
		bool downloadDependencies();

	private:
		std::string m_configDir;
		bool m_ytdlpExists;
		bool m_ffmpegExists;
		NickvisionTubeConverter::Update::Version m_ytdlpLatestVersion;
		NickvisionTubeConverter::Update::Version m_ffmpegLatestVersion;
	};
}

