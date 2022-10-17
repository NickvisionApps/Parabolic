#include "cmdhelpers.hpp"
#include <array>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>

#define READ   0
#define WRITE  1

using namespace NickvisionTubeConverter::Helpers;

FILE* CmdHelpers::popen2(const std::string& command, const std::string& type, int& pid)
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

int CmdHelpers::pclose2(FILE* fp, pid_t pid)
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

std::pair<int, std::string> CmdHelpers::run(const std::string& command, const std::string& type, int& pid)
{
    std::array<char, 128> buffer;
    std::string result{ "" };
    FILE* fp{ popen2(command, type, pid) };
	if (!fp)
	{
		return { -1, result };
	}
	while (!feof(fp))
	{
		if (fgets(buffer.data(), 128, fp) != nullptr)
		{
		    result += buffer.data();
		}
	}
	return { pclose2(fp, pid), result };

}
