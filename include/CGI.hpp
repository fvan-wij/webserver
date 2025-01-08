#pragma once

#include <string>
#include <string_view>
#include <vector>

class CGI
{
public:
	CGI();
	CGI(const CGI &) = delete;
	CGI &operator=(const CGI &) = delete;
	~CGI();

	void verify(std::string_view uri, std::string &param, std::string &body, char *const envp[]);

	void start(char *const envp[]);
	bool poll();
	void kill();

	enum PipeFD
	{
		READ	= 0,
		WRITE	= 1,
		COUNT	= 2,
	};


	const std::string 	&get_buffer() const;
	const int			&get_pipe_fd() const;
	bool				is_running() {return _is_running;};

private:
	bool		_is_running;
	bool 		_is_killed;
	bool 		_has_non_zero_exit;
	std::string _buffer;
	int			_pid;
	int			_pipes[PipeFD::COUNT];
	std::vector<std::string> _argv;


	int32_t _read();
};

