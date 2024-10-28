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

		enum class State {
			ReadingHeaders,
			ReadingBody,
			GeneratingResponse,
			ProcessingCGI,
			UploadingFile,
			FetchingFile,
		};

		void		handle(std::vector<char> data);
		void 		poll_cgi();
		void		poll_upload();
		void		poll_fetch();
		void		respond();
		void		on_data_received(std::vector<char> data);
		void		handle_headers(std::vector<char> data);
		void		handle_body(std::vector<char> data);
		void		generate_response();
		void		parse_file_data(std::vector<char> buffer, t_config& config, std::string_view uri);
		bool		upload_chunk();
		bool		fetch_file(std::string_view path);

		void		start_cgi();

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
		FileUpload			_file;
		CGI					_cgi;
		bool				_b_headers_complete;
		bool				_b_body_complete;
		State				_current_state;
		t_config			_config;

};
