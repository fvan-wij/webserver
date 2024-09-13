#include "CGI.hpp"
#include "meta.hpp"
#include <bits/pthread_stack_min-dynamic.h>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <sys/wait.h>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include "Logger.hpp"





CGI::CGI(const Socket &s) : _socket(s), _is_running(false)
{

}



CGI::~CGI()
{
	// NOTE Maybe use SIGKILL instead?
	if (this->_is_running)
	{
		if (kill(_pid, SIGINT) == -1)
		{
			LOG_ERROR("kill failed " << strerror(errno));
		}
		LOG_DEBUG(_socket << " killed child proc with pid " << _pid);
		int status;
		if (::waitpid(_pid, &status, 0) == -1)
		{
			UNIMPLEMENTED("waitpid failed" << strerror(errno));
		}

		if (WIFEXITED(status))
		{
			LOG_DEBUG("pid: " << _pid << " exited with code: " << WEXITSTATUS(status));
		}
		else if (WIFSIGNALED(status))
		{
			LOG_DEBUG("child with pid: " << _pid << " was signalled : " << strsignal(WTERMSIG(status)));
		}


	}
}



// TODO Max chhildred thing and a LIFO structure that keeps that of the cgi's that wont fit in the Max-child spec
void CGI::start(std::string path)
{
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

	// Child proccess
	else if (_pid == 0)
	{
		// Attach "this" proccess's STDOUT_FILENO to pipe.
		if (dup2(_pipes[int(PipeFD::WRITE)], STDOUT_FILENO) == -1)
		{
			UNIMPLEMENTED("dup2 failed" << strerror(errno));
		}

		close(_pipes[int(PipeFD::WRITE)]);


		char *args[] =
		{
			(char *) path.c_str(),
			(char*) ("1"),
			NULL,
		};

		if (_socket.get_port() == 9091)
		{
			args[1] = (char *) "1";
		}


		if (execvp(args[0], args) == -1)
		{
			UNIMPLEMENTED("execvp failed" << strerror(errno));
		}
		exit(123);
	}
	else
	{
		LOG_NOTICE(_socket << " starting CGI with path: " << path << " PID " << _pid);
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
	else if (WIFEXITED(status))
	{
		if (WIFSIGNALED(status))
		{
			LOG_DEBUG("child with pid: " << _pid << " was signalled : " << strsignal(WTERMSIG(status)));
		}
		else
		{
			LOG_NOTICE(_socket << " CGI exited with code: " << WEXITSTATUS(status));
			// usleep(1);

			// read until the pipe is empty.
			while (_read() == PIPE_READ_SIZE - 1)
			{
				;
			}
			close(_pipes[int(PipeFD::READ)]);
			_is_running = false;
			return true;
		}
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
