#pragma once

#include "Socket.hpp"
#include "CGI.hpp"
#include <string>

enum class Http:int {
	REQUEST = 0,
	RESPONSE = 1,
};

class HttpServer
{
	public:
		HttpServer() = delete;
		HttpServer(const HttpServer &);
		HttpServer &operator=(const HttpServer &) = delete;
		~HttpServer();

		HttpServer(Socket &s);


		void		handle(std::string data);
		void 		poll_cgi();
		void		respond();

		bool		is_ready();
		std::string	get_data() const;
		void		set_request(std::string buffer);
		void		set_response();

		HttpObject	object[2];

	private:
		std::string	_request_buffer;
		bool 		_ready;
		Socket		&_socket;

		CGI			_cgi;

};
