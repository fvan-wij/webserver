#pragma once

#include "Socket.hpp"
#include "CGI.hpp"
#include <string>


class HttpServer
{
	public:
		HttpServer() = delete;
		HttpServer(const HttpServer &);
		HttpServer &operator=(const HttpServer &) = delete;
		~HttpServer();

		HttpServer(Socket &s);


		void		handle(std::string data);

		bool		is_ready();
		std::string	get_data() const;

	private:
		std::string	_request_buffer;
		bool 		_ready;
		Socket		&_socket;

		CGI	_cgi;

};
