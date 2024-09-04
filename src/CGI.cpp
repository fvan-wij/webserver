#include "CGI.hpp"
#include "meta.hpp"
#include <bits/pthread_stack_min-dynamic.h>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <sys/wait.h>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <unistd.h>

CGI::CGI() : _is_running(false)
{

}



void CGI::start(std::string path)
{
	_is_running = true;
	LOG_INFO("starting CGI with path: " << path);
	if (pipe(_pipes) == -1)
	{
		UNIMPLEMENTED("pipe failed" << std::string(strerror(errno)));
	}

	_pid = fork();
	if (_pid == -1)
	{
		UNIMPLEMENTED("fork failed" << strerror(errno));
	}

	// Child proccess
	else if (_pid == 0)
	{
		// Attach "this" proccess's STDOUT_FILENO to pipe.
		if (dup2(_pipes[int(PipeFD::WRITE)], STDOUT_FILENO) == -1)
		{
			UNIMPLEMENTED("dup2 failed" << strerror(errno));
		}

		close(_pipes[int(PipeFD::WRITE)]);
		close(_pipes[int(PipeFD::WRITE)]);


		char *args[] =
		{
			(char *) path.c_str(),
			(char*) ("2"),
			NULL,
		};

		if (execvp(args[0], args) == -1)
		{
			UNIMPLEMENTED("execvp failed" << strerror(errno));
		}
		exit(123);
	}
	close(_pipes[int(PipeFD::WRITE)]);
}




// TODO in the future we should try to implement chucked reading of the pipe.
// incase there is more than 4KB of data.
bool CGI::poll()
{
	if (!_is_running)
		return false;

	int32_t status;
	if (::waitpid(_pid, &status, WNOHANG) == -1)
	{
		UNIMPLEMENTED("waitpid failed" << strerror(errno));
	}

	// NOTE maybe we can just straight up attach the pipe from the CGI to the client's socket_fd.
	if (WIFEXITED(status))
	{
		LOG_INFO("CGI exited with code: " << WEXITSTATUS(status));

		// read until the pipe is empty.
		while (_read() == PIPE_READ_SIZE - 1)
		{
			;
		}
		close(_pipes[int(PipeFD::READ)]);
		_is_running = false;
		return true;
	}
	return false;	
}


const std::string & CGI::get_buffer() const
{
	return _buffer;
}

int32_t CGI::_read()
{
	char buffer[PIPE_READ_SIZE];

	int32_t read_count = read(_pipes[int(PipeFD::READ)], &buffer, PIPE_READ_SIZE - 1);
	if (read_count == -1)
	{
		UNIMPLEMENTED("read failed " << strerror(errno));
	}

	_buffer += buffer;
	return read_count;
}


CGI::~CGI()
{

}
