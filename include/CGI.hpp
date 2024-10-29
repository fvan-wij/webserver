#pragma once


#include <cstdint>
#include <string>
#include <array>

class CGI
{
public:
	CGI();
	CGI(const CGI &) = delete;
	CGI &operator=(const CGI &) = delete;
	~CGI();

	void start(std::array<std::string, 3> arguments);
	bool poll();

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
	std::string _buffer;
	int			_pid;
	int			_pipes[PipeFD::COUNT];


	int32_t _read();
};

