#include "download.hpp"
#include <array>
#include <cstdio>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace NickvisionTubeConverter::Models;

#define READ   0
#define WRITE  1
FILE* popen2(const std::string& command, const std::string& type, int& pid)
{
    pid_t child_pid;
    int fd[2];
    pipe(fd);

    if((child_pid = fork()) == -1)
    {
        perror("fork");
        exit(1);
    }
    /* child process */
    if (child_pid == 0)
    {
        if (type == "r")
        {
            close(fd[READ]);    //Close the READ end of the pipe since the child's fd is write-only
            dup2(fd[WRITE], 1); //Redirect stdout to pipe
        }
        else
        {
            close(fd[WRITE]);    //Close the WRITE end of the pipe since the child's fd is read-only
            dup2(fd[READ], 0);   //Redirect stdin to pipe
        }
        setpgid(child_pid, child_pid); //Needed so negative PIDs can kill children of /bin/sh
        execl("/bin/sh", "/bin/sh", "-c", command.c_str(), NULL);
        exit(0);
    }
    else
    {
        if (type == "r")
        {
            close(fd[WRITE]); //Close the WRITE end of the pipe since parent's fd is read-only
        }
        else
        {
            close(fd[READ]); //Close the READ end of the pipe since parent's fd is write-only
        }
    }
    pid = child_pid;
    if (type == "r")
    {
        return fdopen(fd[READ], "r");
    }
    return fdopen(fd[WRITE], "w");
}

int pclose2(FILE* fp, pid_t pid)
{
    int stat;
    fclose(fp);
    while (waitpid(pid, &stat, 0) == -1)
    {
        if (errno != EINTR)
        {
            stat = -1;
            break;
        }
    }
    return stat;
}

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
	FILE* fp{ popen2(cmd, "r", m_pid) };
	if (!fp)
	{
		return false;
	}
	while (!feof(fp))
	{
		if (fgets(buffer.data(), 128, fp) != nullptr)
		{
			m_log += buffer.data();
		}
	}
	int result{ pclose2(fp, m_pid) };
	if (result != 0)
	{
		m_log += "[Error] Unable to download video";
	}
	return result == 0;
}

void Download::stop()
{
    kill(-m_pid, 9);
}
