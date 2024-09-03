#pragma once


#include <cstdint>
#include <string>

class CGI
{
public:
	CGI();
	CGI(const CGI &) = delete;
	CGI &operator=(const CGI &) = delete;
	~CGI();

	void start(std::string path);
	bool poll();

	enum PipeFD
	{
		READ	= 0,
		WRITE	= 1,
		COUNT	= 2,
	};


	const std::string &get_buffer() const;

private:
	bool		_is_running;
	std::string _buffer;
	int			_pid;
	int			_pipes[PipeFD::COUNT];

	int32_t _read();
};

