#pragma once

#include <string>
#include "MediaFileType.h"
#include "Quality.h"

namespace NickvisionTubeConverter::Models
{
	/// <summary>
	/// A modal representing a video to download
	/// </summary>
	class Download
	{
	public:
		/// <summary>
		/// Constructs a Download
		/// </summary>
		/// <param name="videoUrl">The url of the video to download</param>
		/// <param name="fileType">The file type to download the video as</param>
		/// <param name="saveFolder">The folder to save the download to</param>
		/// <param name="newFilename">The filename to save the download as</param>
		Download(const std::string& videoUrl, const MediaFileType& fileType, const std::string& saveFolder, const std::string& newFilename, Quality quality = Quality::Best);
		/// <summary>
		/// Gets the url of the video to download
		/// </summary>
		/// <returns>The url of the video to download</returns>
		const std::string& getVideoUrl() const;
		/// <summary>
		/// Gets the file type to download the video as
		/// </summary>
		/// <returns>The file type to download the video as</returns>
		const MediaFileType& getMediaFileType() const;
		/// <summary>
		/// Gets the path to save the download to
		/// </summary>
		/// <returns>The path to save the download to</returns>
		std::string getSavePath() const;
		/// <summary>
		/// Gets the quality of the download
		/// </summary>
		/// <returns>The quality of the download</returns>
		Quality getQuality() const;
		/// <summary>
		/// Gets the log from the download
		/// </summary>
		/// <returns>The log from the download</returns>
		const std::string& getLog() const;
		/// <summary>
		/// Downloads the video
		/// </summary>
		/// <returns>True if the download was successful, else false</returns>
		bool download();

	private:
		std::string m_videoUrl;
		MediaFileType m_fileType;
		std::string m_path;
		Quality m_quality;
		std::string m_log;
	};
}

