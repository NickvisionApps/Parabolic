#include "download.hpp"
#include <array>
#include <cstdio>

using namespace NickvisionTubeConverter::Models;

Download::Download(const std::string& videoUrl, const MediaFileType& fileType, const std::string& saveFolder, const std::string& newFilename, Quality quality) : m_videoUrl{ videoUrl }, m_fileType{ fileType }, m_path{ saveFolder + "/" + newFilename }, m_quality{ quality }, m_log { "" }
{

}

const std::string& Download::getVideoUrl() const
{
	return m_videoUrl;
}

bool Download::checkIfVideoUrlValid() const
{
    std::string cmd{ "yt-dlp -j " + m_videoUrl };
    std::array<char, 128> buffer;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe)
	{
		return false;
	}
	while (!feof(pipe))
	{
	    fgets(buffer.data(), 128, pipe);
	}
	return pclose(pipe) == 0;
}

const MediaFileType& Download::getMediaFileType() const
{
	return m_fileType;
}

std::string Download::getSavePath() const
{
	return m_path + m_fileType.toDotExtension();
}

Quality Download::getQuality() const
{
	return m_quality;
}

const std::string& Download::getLog() const
{
	return m_log;
}

bool Download::download()
{
    std::string cmd{ "" };
	if (m_fileType.isVideo())
	{
	    std::string format{ m_quality == Quality::Best ? "bv*+ba/b" : m_quality == Quality::Worst ? "wv*+wa/w" : "" };
		cmd = "yt-dlp --format " + format + " --remux-video " + m_fileType.toString() + " \"" + m_videoUrl + "\" -o \"" + m_path + ".%(ext)s\"";
	}
	else
	{
		cmd = "yt-dlp --extract-audio --audio-format " + m_fileType.toString() + " --audio-quality " + (m_quality == Quality::Best ? "0" : m_quality == Quality::Good ? "5" : "10") + " \"" + m_videoUrl + "\" -o \"" + m_path + ".%(ext)s\"";
	}
	m_log = "URL: " + m_videoUrl + "\nPath: " + getSavePath() + "\nQuality: " + std::to_string(static_cast<int>(m_quality)) + "\n\n";
	std::array<char, 128> buffer;
	FILE* pipe = popen(cmd.c_str(), "r");
	if (!pipe)
	{
		m_log += "[Error] Unable to run command";
		return false;
	}
	while (!feof(pipe))
	{
		if (fgets(buffer.data(), 128, pipe) != nullptr)
		{
			m_log += buffer.data();
		}
	}
	int result{ pclose(pipe) };
	if (result != 0)
	{
		m_log += "[Error] Unable to download video";
	}
	return result == 0;
}

void Download::stop()
{

}
