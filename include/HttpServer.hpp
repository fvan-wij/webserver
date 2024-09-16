#pragma once

#include "Socket.hpp"
#include "CGI.hpp"
#include <string>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HandlerFactory.hpp"
#include "Config.hpp"
#include <charconv>

class HttpServer
{
	public:
		HttpServer();
		HttpServer(const HttpServer &);
		HttpServer &operator=(const HttpServer &) = delete;
		~HttpServer();

		// HttpServer(Socket &s);

		enum class State {
			ReadingHeaders,
			ReadingBody,
			GeneratingResponse,
			ProcessingCGI,
		};

		void		handle(std::vector<char> data);
		void 		poll_cgi();
		void		respond();
		void		on_data_received(std::vector<char> data);
		void		handle_headers(std::vector<char> data);
		void		handle_body(std::vector<char> data);
		void		generate_response();

		bool		is_ready();
		std::string	get_data();

		HttpResponse	response;
		HttpRequest		request;

	private:
		std::string	_header_buffer;
		std::vector<char> _body_buffer;
		// Socket		&_socket;
		CGI			_cgi;
		bool		_b_headers_complete;
		bool		_b_body_complete;
		State		_current_state;

};
