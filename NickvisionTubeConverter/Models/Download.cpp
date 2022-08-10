#include "Download.h"
#include <array>
#include <cstdio>
#include <QStandardPaths>

namespace NickvisionTubeConverter::Models
{
	Download::Download(const std::string& videoUrl, const MediaFileType& fileType, const std::string& saveFolder, const std::string& newFilename) : m_videoUrl{ videoUrl }, m_fileType{ fileType }, m_path{ saveFolder + "/" + newFilename }
	{

	}

	const std::string& Download::getVideoUrl() const
	{
		return m_videoUrl;
	}

	const MediaFileType& Download::getMediaFileType() const
	{
		return m_fileType;
	}

	std::string Download::getSavePath() const
	{
		return m_path + m_fileType.toDotExtension();
	}

	std::pair<bool, std::string> Download::download() const
	{
		std::string cmd{ QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString() + "/yt-dlp.exe "};
		if (m_fileType.isVideo())
		{
			cmd += "--remux-video " + m_fileType.toString() + " \"" + m_videoUrl + "\" -o \"" + m_path + ".%(ext)s\"";
		}
		else
		{
			cmd += "--extract-audio --audio-format " + m_fileType.toString() + " \"" + m_videoUrl + "\" -o \"" + m_path + ".%(ext)s\"";
		}
		std::string cmdOutput{ "===Starting Download===\nURL: " + m_videoUrl + "\nPath: " + getSavePath() + "\n\n"};
		std::array<char, 128> buffer;
		FILE* pipe = _popen(cmd.c_str(), "r");
		if (!pipe)
		{
			return std::make_pair(false, "[Error] Unable to run command");
		}
		while (!feof(pipe))
		{
			if (fgets(buffer.data(), 128, pipe) != nullptr)
			{
				cmdOutput += buffer.data();
			}
		}
		int result{ _pclose(pipe) };
		if (result != 0)
		{
			cmdOutput += "[Error] Unable to download video\n";
		}
		cmdOutput += "==========\n\n";
		return std::make_pair(result == 0, cmdOutput);
	}
}