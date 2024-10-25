#pragma once

#include "Socket.hpp"
#include "CGI.hpp"
#include <string>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HandlerFactory.hpp"
#include "Config.hpp"
#include <charconv>

typedef struct FileUpload 
{
	std::string			filename;
	std::string			path;
	std::vector<char> 	data;
	size_t				bytes_uploaded;
	bool				finished;
} FileUpload;

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
			UploadingFile,
		};

		void		handle(std::vector<char> data);
		void 		poll_cgi();
		void		poll_upload();
		void		respond();
		void		on_data_received(std::vector<char> data);
		void		handle_headers(std::vector<char> data);
		void		handle_body(std::vector<char> data);
		void		generate_response();
		void		parse_file_data(std::vector<char> buffer, t_config& config, std::string_view uri);
		bool		upload_chunk();

		void		start_cgi();

		// std::optional<size_t> validate_content_length(); //413 Payload Too Large
		// std::optional<size_t> validate_path();
		// std::optional<size_t> validate_content_type();
		// std::optional<size_t> validate_methods();
		// std::optional<size_t> validate_version(); //505 HTTP Version Not Supported
		// std::optional<size_t> validate_host(); //400 Bad Request
		// std::optional<size_t> validate_method_body_consistency();
		// std::optional<size_t> validate_multipart_form_data();

		bool		is_ready();
		bool		is_cgi_running() {return _cgi.is_running();};
		std::string	get_data();
		t_config	get_config();
		int			get_pipe_fd();

		HttpResponse	response;
		HttpRequest		request;

	private:
		std::string			_header_buffer;
		std::vector<char> 	_body_buffer;
		// std::vector<char> 	_file_data;
		FileUpload			_file;
		CGI					_cgi;
		bool				_b_headers_complete;
		bool				_b_body_complete;
		State				_current_state;
		t_config			_config;

};
