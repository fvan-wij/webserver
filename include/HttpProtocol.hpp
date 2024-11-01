#pragma once

#include "CGI.hpp"
#include <string>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Config.hpp"

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


		//			Methods
		void		handle(std::vector<char> data);
		void		respond();
		// void		handle_headers(std::vector<char> data);
		// void		handle_body(std::vector<char> data);
		void		generate_response();
		void		parse_file_data(std::vector<char> buffer, t_config& config, std::string_view uri);
		void		build_error_response(int error_code, std::string_view message);

		//			File uploading/fetching
		bool		upload_chunk();
		bool		fetch_file(std::string_view path);
		void		poll_upload();
		void		poll_fetch();

		//			CGI
		void		start_cgi(char *envp[]);
		void 		poll_cgi();

		//			Bools
		bool		is_ready();
		bool		is_cgi_running() {return _cgi.is_running();};

		//			Getters
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
		bool				_b_headers_complete; // Can be moved to request object
		bool				_b_body_complete;// Can be moved to request object
		State				_current_state;
		t_config			_config;

};
