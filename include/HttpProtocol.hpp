#pragma once

#include "Socket.hpp"
#include "CGI.hpp"
#include <string>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HandlerFactory.hpp"
#include "Config.hpp"
#include <charconv>

class HttpProtocol
{
	public:
		HttpProtocol();
		HttpProtocol(t_config &config);
		HttpProtocol(const HttpProtocol &);
		HttpProtocol &operator=(const HttpProtocol &);
		~HttpProtocol();

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

		// std::optional<size_t> validate_content_length(); //413 Payload Too Large
		// std::optional<size_t> validate_path();
		// std::optional<size_t> validate_content_type();
		// std::optional<size_t> validate_methods();
		// std::optional<size_t> validate_version(); //505 HTTP Version Not Supported
		// std::optional<size_t> validate_host(); //400 Bad Request
		// std::optional<size_t> validate_method_body_consistency();
		// std::optional<size_t> validate_multipart_form_data();

		bool		is_ready();
		std::string	get_data();
		t_config	get_config();

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
		t_config	_config;

};
