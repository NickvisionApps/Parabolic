#include "DependencyManager.h"
#include <filesystem>
#include <QStandardPaths>
#include "../Helpers/CurlHelpers.h"
#include "../Helpers/ZipHelpers.h"
#include "../Models/Configuration.h"

using namespace NickvisionTubeConverter::Helpers;
using namespace NickvisionTubeConverter::Models;

namespace NickvisionTubeConverter::Models
{
	DependencyManager::DependencyManager() : m_configDir{ QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString() }, m_ytdlpLatestVersion{ "2022.8.8" }, m_ffmpegLatestVersion{ "5.1.0" }
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
		Configuration& configuration{ Configuration::getInstance() };
		//==ytdlp==//
		if (!m_ytdlpExists || m_ytdlpLatestVersion > configuration.getYtdlpVersion())
		{
			if (!CurlHelpers::downloadFile("https://github.com/yt-dlp/yt-dlp/releases/download/2022.08.08/yt-dlp.exe", m_configDir + "/yt-dlp.exe"))
			{
				return false;
			}
			//Save Config
			m_ytdlpExists = true;
			configuration.setYtdlpVersion(m_ytdlpLatestVersion);
			configuration.save();
		}
		//==ffmpeg==//
		if (!m_ffmpegExists || m_ffmpegLatestVersion > configuration.getFfmpegVersion())
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
			//Save Config
			m_ffmpegExists = true;
			configuration.setFfmpegVersion(m_ffmpegLatestVersion);
			configuration.save();
		}
		return true;
	}
}