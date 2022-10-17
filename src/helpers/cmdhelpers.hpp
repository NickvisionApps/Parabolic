#pragma once

#include <string>
#include <utility>
#include <sys/types.h>

namespace NickvisionTubeConverter::Helpers::CmdHelpers
{
	/**
	 * A popen function that creates a pipe with a pid
	 *
	 * @param command The command to run
	 * @param type The type for the stream (r or w)
	 * @param pid A reference to an int variable to store the pid
	 */
	FILE* popen2(const std::string& command, const std::string& type, int& pid);
	/**
	 * A pclose function that closes the pipe
	 *
	 * @param fp The FILE*
	 * @param pid The pid of the pipe to close
	 */
	int pclose2(FILE* fp, pid_t pid);
	/**
	 * Runs a command
	 *
	 * @param command The command to run
	 * @param type The type for the stream (r or w)
	 * @param pid A reference to an int variable to store the pid
	 */
	std::pair<int, std::string> run(const std::string& command, const std::string& type, int& pid);
}