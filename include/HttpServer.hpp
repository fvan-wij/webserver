#pragma once

#include "Socket.hpp"
#include "CGI.hpp"
#include <string>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HandlerFactory.hpp"

class HttpServer
{
	public:
		HttpServer(const Socket &s);
		HttpServer(const HttpServer &);
		HttpServer &operator=(const HttpServer &);
		~HttpServer();

		HttpServer() = delete;


		void		handle(HttpRequest &request);
		void 		poll_cgi();
		void		respond();

		bool		is_ready();
		std::string	get_data();

		HttpResponse	response;

	private:
		std::string	_request_buffer;
		const Socket		&_socket;
		CGI			_cgi;

};
