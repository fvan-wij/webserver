#include <CGIHandler.hpp>
#include <cstdlib>
#include <unistd.h>
#include <Logger.hpp>
#include <meta.hpp>
#include <cerrno>
#include <string.h>
#include <iostream>

CGIHandler::CGIHandler(ConnectionManager &cm) : _cm(cm)
{

}

CGIHandler::~CGIHandler()
{

}

void	CGIHandler::handle_cgi(short events)
{

}

void	CGIHandler::read_pipe()
{

}

void	CGIHandler::child_process(std::vector<const char*> args, char *const envp[])
{
	// Attach "this" process's STDOUT_FILENO to pipe.
	if (dup2(_pipes[PipeFD::WRITE], STDOUT_FILENO) == -1)
	{
		UNIMPLEMENTED("dup2 failed" << strerror(errno));
	}

	close(_pipes[PipeFD::WRITE]);

	if (execve(args[0], const_cast<char* const*>(args.data()), envp) == -1)
	{
		UNIMPLEMENTED("execvp failed" << strerror(errno));
	}
	exit(123);
}

void	CGIHandler::execute_cgi(std::vector<const char*> args, char *const envp[])
{
	args.push_back(nullptr);
	_is_running = true;
	if (pipe(_pipes) == -1)
	{
		UNIMPLEMENTED("pipe failed" << strerror(errno));
	}

	_pid = fork();
	if (_pid == -1)
	{
		UNIMPLEMENTED("fork failed" << strerror(errno));
	}
	else if (_pid == 0)
	{
		child_process(args, envp);
	}
	close(_pipes[PipeFD::WRITE]);
}