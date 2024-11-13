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
		void		generate_response();
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
		bool		is_cgi_running();

		//			Getters
		std::string	get_data();
		Config		get_config();
		int			get_pipe_fd();
		State		get_state();

		HttpResponse			response;
		HttpRequest				request;

	private:
		FileUpload				_file;
		CGI						_cgi;
		State					_state;
		Config					_config;

};
