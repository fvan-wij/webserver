#pragma once

#include "Socket.hpp"
#include "CGI.hpp"
#include <string>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HandlerFactory.hpp"
#include "Config.hpp"

class HttpServer
{
	public:
		HttpServer();
		HttpServer(const HttpServer &);
		HttpServer &operator=(const HttpServer &) = delete;
		~HttpServer();

		// HttpServer(Socket &s);


		void		handle(HttpRequest &request);
		void 		poll_cgi();
		void		respond();

		bool		is_ready();
		bool		is_request_valid(HttpRequest &request, t_config &config);
		std::string	get_data();

		HttpResponse	response;

	private:
		std::string	_request_buffer;
		// Socket		&_socket;
		CGI			_cgi;

};
