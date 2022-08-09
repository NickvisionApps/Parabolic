#include "DependencyManager.h"
#include <filesystem>
#include <QStandardPaths>
#include "../Helpers/CurlHelpers.h"
#include "../Helpers/ZipHelpers.h"

using namespace NickvisionTubeConverter::Helpers;

namespace NickvisionTubeConverter::Models
{
	DependencyManager::DependencyManager() : m_configDir{ QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString() }
	{
		if (!std::filesystem::exists(m_configDir))
		{
			std::filesystem::create_directories(m_configDir);
		}
		m_ytdlpExists = std::filesystem::exists(m_configDir + "/yt-dlp.exe");
		m_ffmpegExists = std::filesystem::exists(m_configDir + "/ffmpeg.exe") && std::filesystem::exists(m_configDir + "/ffplay.exe") && std::filesystem::exists(m_configDir + "/ffprobe.exe");
	}

	bool DependencyManager::getYtdlpExists() const
	{
		return m_ytdlpExists;
	}

	bool DependencyManager::getFfmpegExists() const
	{
		return m_ffmpegExists;
	}

	bool DependencyManager::downloadDependencies()
	{
		if (m_ytdlpExists && m_ffmpegExists)
		{
			return true;
		}
		//==ytdlp==//
		if (!m_ytdlpExists)
		{
			if (!CurlHelpers::downloadFile("https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp.exe", m_configDir + "/yt-dlp.exe"))
			{
				return false;
			}
			m_ytdlpExists = true;
		}
		//==ffmpeg==//
		if (!m_ffmpegExists)
		{
			//Download ffmpeg zip
			if (!CurlHelpers::downloadFile("https://github.com/GyanD/codexffmpeg/releases/download/5.1/ffmpeg-5.1-full_build.zip", m_configDir + "/ffmpeg.zip"))
			{
				return false;
			}
			//Extract exe files from ffmpeg zip
			ZipHelpers::extractEntryFromZip(m_configDir + "/ffmpeg.zip", "ffmpeg-5.1-full_build/bin/ffmpeg.exe", m_configDir + "/ffmpeg.exe");
			ZipHelpers::extractEntryFromZip(m_configDir + "/ffmpeg.zip", "ffmpeg-5.1-full_build/bin/ffplay.exe", m_configDir + "/ffplay.exe");
			ZipHelpers::extractEntryFromZip(m_configDir + "/ffmpeg.zip", "ffmpeg-5.1-full_build/bin/ffprobe.exe", m_configDir + "/ffprobe.exe");
			//Cleanup
			std::filesystem::remove(m_configDir + "/ffmpeg.zip");
			m_ffmpegExists = true;
		}
		return true;
	}
}