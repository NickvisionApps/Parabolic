#pragma once

#include <mutex>
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
	 * Statuses for when a download is checked
	 */
	enum class DownloadCheckStatus
	{
		Valid = 0,
		EmptyVideoUrl,
		InvalidVideoUrl,
		EmptySaveFolder,
		InvalidSaveFolder
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
		const std::string& getVideoUrl();
		/**
		 * Checks if the download is valid
		 *
		 * @returns The DownloadCheckStatus
		 */
		DownloadCheckStatus getValidStatus();
		/**
		 * Gets the file type to download the video as
		 *
		 * @returns The file type to download the video as
		 */
		const MediaFileType& getMediaFileType();
		/**
		 * Gets the path to save the download to
		 *
		 * @returns The path to save the download to
		 */
		std::string getSavePath();
		/**
		 * Gets the quality of the download
		 *
		 * @returns The quality of the download
		 */
		Quality getQuality();
		/**
		 * Gets the log from the download
		 *
		 * @returns The log from the download
		 */
		const std::string& getLog();
		/**
		 * Gets whether or not the download is done or not
		 *
		 * @returns True if done, else false
		 */
		bool getIsDone();
		/**
		 * Downloads the video
		 *
		 * @param embedMetadata Whether or not to embed metadata into the download file
		 * @returns True if successful, else false
		 */
		bool download(bool embedMetadata);
		/**
		 * Stops the download
		 */
		void stop();

	private:
		std::mutex m_mutex;
		std::string m_videoUrl;
		MediaFileType m_fileType;
		std::string m_path;
		Quality m_quality;
		std::string m_log;
		bool m_isValidUrl;
		bool m_isDone;
		int m_pid;
		/**
		 * Gets the path to save the download to (without the dot extension)
		 *
		 * @returns The path to save the download to (without the dot extension)
		 */
		const std::string& getSavePathWithoutExtension();
		/**
		 * Downloads the title from the video
		 *
		 * @returns The title from the video
		 */
		std::string getTitleFromVideo();
		/**
		 * Sets the log of the download
		 *
		 * @param log The new log
		 */
		void setLog(const std::string& log);
		/**
		 * Sets whether or not the download is done
		 *
		 * @param isDone True for done, else false
		 */
		void setIsDone(bool isDone);
	};
}