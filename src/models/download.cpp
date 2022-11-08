#include "download.hpp"
#include <algorithm>
#include <filesystem>
#include <utility>
#include <signal.h>
#include "../helpers/cmdhelpers.hpp"
#include "../helpers/translation.hpp"

using namespace NickvisionTubeConverter::Helpers;
using namespace NickvisionTubeConverter::Models;

Download::Download(const std::string& videoUrl, const MediaFileType& fileType, const std::string& saveFolder, const std::string& newFilename, Quality quality, Subtitles subtitles) : m_videoUrl{ videoUrl }, m_fileType{ fileType }, m_path{ saveFolder + "/" + newFilename }, m_quality{ quality }, m_subtitles{ subtitles }, m_log { "" }, m_isValidUrl{ false }, m_isDone{ false }
{
    std::string videoTitle{ getTitleFromVideo() };
    m_isValidUrl = !videoTitle.empty();
    if(newFilename.empty())
    {
        m_path += videoTitle;
    }
}

const std::string& Download::getVideoUrl()
{
    std::lock_guard<std::mutex> lock{ m_mutex };
	return m_videoUrl;
}

DownloadCheckStatus Download::getValidStatus()
{
    if(getVideoUrl().empty())
    {
        return DownloadCheckStatus::EmptyVideoUrl;
    }
    if(!m_isValidUrl)
    {
        return DownloadCheckStatus::InvalidVideoUrl;
    }
    std::filesystem::path downloadPath{ getSavePath() };
    if(downloadPath.parent_path() == "/")
    {
        return DownloadCheckStatus::EmptySaveFolder;
    }
    if(!std::filesystem::exists(downloadPath.parent_path()))
    {
        return DownloadCheckStatus::InvalidSaveFolder;
    }
    return DownloadCheckStatus::Valid;
}

const MediaFileType& Download::getMediaFileType()
{
    std::lock_guard<std::mutex> lock{ m_mutex };
	return m_fileType;
}

std::string Download::getSavePath()
{
    std::lock_guard<std::mutex> lock{ m_mutex };
	return m_path + m_fileType.toDotExtension();
}

Quality Download::getQuality()
{
    std::lock_guard<std::mutex> lock{ m_mutex };
	return m_quality;
}

Subtitles Download::getSubtitles()
{
    std::lock_guard<std::mutex> lock{ m_mutex };
	return m_subtitles;
}

const std::string& Download::getLog()
{
    std::lock_guard<std::mutex> lock{ m_mutex };
	return m_log;
}

bool Download::getIsDone()
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    return m_isDone;
}

bool Download::download(bool embedMetadata)
{
    std::string cmd{ "" };
	if (getMediaFileType().isVideo())
	{
	    std::string format{ getQuality() == Quality::Best ? "bv*+ba/b" : getQuality() == Quality::Good ? "\"bv*[height<=720]+ba/b[height<=720]\"" : getQuality() == Quality::Worst ? "wv*+wa/w" : "" };
		cmd = "yt-dlp --format " + format + " --remux-video " + getMediaFileType().toString() + " \"" + getVideoUrl() + "\" -o \"" + getSavePathWithoutExtension() + ".%(ext)s\"";
		if(getSubtitles() != Subtitles::None)
	    {
	        std::string subtitles{ getSubtitles() == Subtitles::VTT ? "vtt" : "srv3" };
	        if(getContainsSubtitles())
	        {
	            cmd += " --embed-subs --all-subs --sub-format " + subtitles;
	        }
	        else
	        {
	            cmd += " --write-auto-sub --embed-subs --all-subs --sub-format " + subtitles;
	        }
	    }
	}
	else
	{
		cmd = "yt-dlp --extract-audio --audio-format " + getMediaFileType().toString() + " --audio-quality " + (getQuality() == Quality::Best ? "0" : getQuality() == Quality::Good ? "5" : "10") + " \"" + getVideoUrl() + "\" -o \"" + getSavePathWithoutExtension() + ".%(ext)s\"";
	}
	if(embedMetadata)
	{
	    cmd += " --add-metadata --embed-thumbnail";
	}
	setLog("URL: " + getVideoUrl() + "\nPath: " + getSavePath() + "\nQuality: " + std::to_string(static_cast<int>(getQuality())) + "\n");
	std::pair<int, std::string> result{ CmdHelpers::run(cmd, "r", m_pid) };
	{
	    std::lock_guard<std::mutex> lock{ m_mutex };
	    m_log += result.second;
	}
	setIsDone(true);
	if (result.first != 0)
	{
	    std::lock_guard<std::mutex> lock{ m_mutex };
		m_log += _("\n[Error] Unable to download video");
	    return false;
	}
	return true;
}

void Download::stop()
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    kill(-m_pid, 9);
    m_isDone = true;
}

const std::string& Download::getSavePathWithoutExtension()
{
    std::lock_guard<std::mutex> lock{ m_mutex };
	return m_path;
}

std::string Download::getTitleFromVideo()
{
    int pid;
    std::string title{ CmdHelpers::run("yt-dlp --get-title --skip-download " + getVideoUrl(), "r", pid).second };
	title.erase(std::remove(title.begin(), title.end(), '\n'), title.cend());
	return title;
}

bool Download::getContainsSubtitles()
{
    int pid;
    std::string output{ CmdHelpers::run("yt-dlp --list-subs " + getVideoUrl(), "r", pid).second };
	return output.find("has no subtitles") == std::string::npos;
}

void Download::setLog(const std::string& log)
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    m_log = log;
}

void Download::setIsDone(bool isDone)
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    m_isDone = isDone;
}

