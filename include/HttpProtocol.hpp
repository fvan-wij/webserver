#pragma once

#include "CGI.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Config.hpp"

#include <string>

class HttpProtocol
{
	public:
		HttpProtocol();
		HttpProtocol(Config &config);
		HttpProtocol(const HttpProtocol &);
		HttpProtocol &operator=(const HttpProtocol &);
		~HttpProtocol();

		//			Methods
		void		handle(std::vector<char>& data);
		void		generate_response();
		void		parse_file_data(std::vector<char> buffer, Config& config, std::string_view uri);
		void		build_error_response(int error_code, std::string_view message);
		void		parse_data(std::vector<char>& data);

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
		// TODO Put this in .cpp file
		bool		is_cgi_running() {return _cgi.is_running();};

		//			Getters
		std::string	get_data();
		Config	get_config();
		int			get_pipe_fd();
		State		get_state();

		HttpResponse			response;
		HttpRequest				request;

	private:
		FileUpload				_file;
		CGI						_cgi;
		bool					_b_headers_complete; 	// Can be moved to request object
		bool					_b_body_complete;		// Can be moved to request object
		State					_state;
		Config				_config;

};
