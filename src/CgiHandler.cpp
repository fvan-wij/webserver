#include "CgiHandler.hpp"
#include "HttpRequest.hpp"
#include "Logger.hpp"
#include "meta.hpp"
#include <bits/pthread_stack_min-dynamic.h>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iterator>
#include <string_view>
#include <sys/wait.h>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <vector>
#include <sys/poll.h>

static char	*find_path(char *const envp[])
{
	int		index;

	index = 0;
	while (envp[index])
	{
		if (::strncmp(envp[index], "PATH=", 5) == 0)
			return (envp[index] + 5);
		index++;
	}
	return (NULL);
}

static std::vector<std::string> split(const std::string& str, const char& ch) 
{
	std::string next;
	std::vector<std::string> result;

	// For each character in the string
	for (std::string::const_iterator it = str.begin(); it != str.end(); it++)
	{
		// If we've hit the terminal character
		if (*it == ch)
		{
			// If we have some characters accumulated
			if (!next.empty())
			{
				// Add them to the result vector
				result.push_back(next);
				next.clear();
			}
		}
		else
		{
			// Accumulate the next character into the sequence
			next += *it;
		}
	}
	if (!next.empty())
		result.push_back(next);
	return result;
}

static std::string find_cgi_binary(const std::string exec_name, char *const envp[])
{
	const char* path = find_path(envp);

	if (!path)
	{
		LOG_ERROR("PATH not found in envp");
		throw HttpException(500, "Internal Server Error");
	}

	std::vector<std::string> paths = split(path, ':');

	for (std::string &s : paths)
	{
		std::string path = s + "/" + exec_name;
		if (::access(path.c_str(), X_OK) == 0)
			return path;
	}
	LOG_ERROR(exec_name << " not found in $PATH");
	throw HttpException(500, "Internal Server Error");
}

CgiHandler::CgiHandler(std::string path, std::string url_params, std::string& body_buff, char **envp) : _is_running(false), _is_finished(false), _is_killed(false), _error(false)
{
	verify(path, url_params, body_buff, envp);
	start(envp);
}

void CgiHandler::handle_cgi(short revents)
{
	// if (_is_running)
	// 	return;

	try
	{
		if (revents & POLLIN)
		{
			_read_pipe();
		}
	}
	catch (HttpException& e)
	{
		LOG_ERROR("Error reading pipe!");
	}

}

void CgiHandler::verify(std::string_view uri, std::string &param, std::string &body, char *const envp[])
{
	// just hardcode python3 for now...
	const std::string path = find_cgi_binary("python3", envp);

	
	// argv[0] path to python binary
	_argv.push_back(path);
	// argv[1] path to exec
	_argv.push_back(std::string(uri));
	// argv[2] URL parameters
	_argv.push_back(param);
	// argv[3] POST body
	_argv.push_back(body);
}



void CgiHandler::start(char *const envp[])
{
	_is_running = true;


	std::string args_printable;

	for (auto& var : _argv)
	{
		args_printable += var;
		if (var != *std::prev(_argv.end()))
			args_printable += ", ";
	}
	LOG_NOTICE("starting CgiHandler @: " + args_printable);

	if (pipe(_pipes) == -1)
	{
		throw HttpException(500, "Internal Server Error");
	}

	_pid = fork();
	if (_pid == -1)
	{
		throw HttpException(500, "Internal Server Error");
	}

	// Child process
	else if (_pid == 0)
	{
		// Attach "this" process's STDOUT_FILENO to pipe.
		if (dup2(_pipes[PipeFD::WRITE], STDOUT_FILENO) == -1)
		{
			throw HttpException(500, "Internal Server Error");
		}

		if (close(_pipes[PipeFD::WRITE]) == -1)
		{
			throw HttpException(500, "Internal Server Error");
		}


		const char **argv = new const char* [_argv.size() + 1];
		for (size_t i = 0; i < _argv.size(); i++)
		{
			argv[i] = _argv.at(i).c_str();
		}
		argv[_argv.size()] = NULL;


		if (execve(argv[0], (char* const*) argv, envp) == -1)
		{
			throw HttpException(500, "Internal Server Error");
		}
	}
	if (close(_pipes[PipeFD::WRITE]) == -1)
	{
		throw HttpException(500, "Internal Server Error");
	}
}


bool CgiHandler::poll()
{
	if (!_is_running)
		return false;

	int options = WNOHANG;
	// incase the process has been sent a kill signal we should wait for it to "finish" to prevent zombie procs.
	if (_is_killed)
		options = 0;

	int32_t status;
	int32_t return_code = ::waitpid(_pid, &status, options);
	if (return_code == -1)
	{
		throw HttpException(500, "Internal Server Error");
	}
	else if (return_code == _pid)
	{
		if (WIFEXITED(status))
		{
			LOG_DEBUG("CgiHandler exited with code: " << WEXITSTATUS(status));
		}
		else if (WIFSIGNALED(status))
		{
			LOG_DEBUG("CgiHandler received " << strsignal(WTERMSIG(status)) << " with code: " << WTERMSIG(status));
		}
		_is_running = false;
		if (close(_pipes[PipeFD::READ]) == -1)
		{
			throw HttpException(500, "Internal Server Error");
		}
		if (WEXITSTATUS(status))
		{
			_error = true;
			throw HttpException(500, "Internal Server Error");
		}
		return true;
	}
	return false;
}


void CgiHandler::kill()
{
	if (!_is_running || _error)
		return;
	if (::kill(_pid, SIGTERM) == -1)
	{
		LOG_ERROR("Failed" << strerror(errno));
	}
	else
	{
		_is_killed = true;
		bool status = this->poll();
		LOG_NOTICE("Killed CgiHandler : " << status);
	}
}

const std::string& CgiHandler::get_buffer() const
{
	return _buffer;
}

const int& CgiHandler::get_pipe_fd() const
{
	return _pipes[READ];
}

int32_t CgiHandler::_read_pipe()
{
	char buffer[PIPE_READ_SIZE];

	int32_t read_count = read(_pipes[PipeFD::READ], &buffer, PIPE_READ_SIZE - 1);
	if (read_count == -1)
	{
		throw HttpException(500, "Internal Server Error");
	}
	else if (read_count == 0)
	{
		LOG_NOTICE("CgiHandler finished!");
		_is_finished = true;
	}
	_buffer += buffer;
	return read_count;
}


CgiHandler::~CgiHandler()
{

}
