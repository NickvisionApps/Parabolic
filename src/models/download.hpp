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
		 * Checks if the video url is valid
		 *
		 * @returns True if valid, else false
		 */
		bool checkIfVideoUrlValid();
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
		 * @returns True if successful, else false
		 */
		bool download();
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
		bool m_done;
		int m_pid;
		/**
		 * Gets the path to save the download to (without the dot extension)
		 *
		 * @returns The path to save the download to (without the dot extension)
		 */
		const std::string& getSavePathWithoutExtension();
		void setLog(const std::string& log);
		void setDone(bool done);
	};
}