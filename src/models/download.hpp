#pragma once

#include <string>
#include "mediafiletype.hpp"

namespace NickvisionTubeConverter::Models
{
	/**
	 * Qualities for a Download
	 */
	enum class Quality
	{
		Best = 0,
		Good,
		Worst
	};

	/**
	 * A model of a video download
	 */
	class Download
	{
	public:
		/**
		 * Constructs a Download
		 *
		 * @param videoUrl The url of the video to download
		 * @param fileType The file type to download the video as
		 * @param saveFolder The folder to save the download to
		 * @param newFilename The filename to save the download as
		 * @param quality The quality of the download
		 */
		Download(const std::string& videoUrl, const MediaFileType& fileType, const std::string& saveFolder, const std::string& newFilename, Quality quality = Quality::Best);
		/**
		 * Gets the url of the video to download
		 *
		 * @returns The url of the video to download
		 */
		const std::string& getVideoUrl() const;
		/**
		 * Gets the file type to download the video as
		 *
		 * @returns The file type to download the video as
		 */
		const MediaFileType& getMediaFileType() const;
		/**
		 * Gets the path to save the download to
		 *
		 * @returns The path to save the download to
		 */
		std::string getSavePath() const;
		/**
		 * Gets the quality of the download
		 *
		 * @returns The quality of the download
		 */
		Quality getQuality() const;
		/**
		 * Gets the log from the download
		 *
		 * @returns The log from the download
		 */
		const std::string& getLog() const;
		/**
		 * Downloads the video
		 *
		 * @returns True if successful, else false
		 */
		bool download();

	private:
		std::string m_videoUrl;
		MediaFileType m_fileType;
		std::string m_path;
		Quality m_quality;
		std::string m_log;
	};
}