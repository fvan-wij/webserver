#pragma once


#include "Socket.hpp"
#include <cstdint>
#include <string>

class CGI
{
public:
	CGI(const Socket &s);
	CGI(const CGI &) = delete;
	CGI &operator=(const CGI &) = delete;
	~CGI();

	CGI() = delete;

	void start(std::string path);
	bool poll();

	enum class PipeFD
	{
		READ	= 0,
		WRITE	= 1,
		COUNT	= 2,
	};


	const std::string &get_buffer() const;

private:
	const Socket &_socket;
	bool		_is_running;
	std::string _buffer;
	int			_pid;
	int			_pipes[int(PipeFD::COUNT)];

	int32_t _read();
};

