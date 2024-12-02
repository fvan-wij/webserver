#pragma once

#include <string>
#include <unordered_map>
#include <vector>

enum {
	NOT_READY = false,
	READY = true,
};

enum class ResponseType:int {
	Regular = 0,
	Upload,
	Delete,
	CGI,
	Fetch,
	Error,
	Autoindex,
	Unknown,
};

const static std::unordered_map<int, std::string> HTTP_REDIRECTION =
{
	{300, "Multiple Choices"},
	{301, "Moved Permanently"},
	{302, "Found"},
	{303, "See Other"},
	{304, "Not Modified"},
	{305, "Use Proxy"},
	{306, "Switch Proxy"},
	{307, "Temporary Redirect"},
	{308, "Permanent Redirect"},
};

class HttpResponse
{
	public:
		HttpResponse();
		~HttpResponse();
		void			set_status_code(const int status);
		void			set_status_mssg(const std::string &mssg);
		void			set_body(const std::string& body);
		void			set_state(bool state){_ready = state;};
		void			set_type(ResponseType type){_type = type;};
		void			set_path(std::string path){_path = path;};
		void			set_streamcount(size_t bytes) {_streamcount = bytes;};
		void			set_virtual_host(std::string virtual_host) {_virtual_host = virtual_host;};
		void			set_server(std::string server) {_server = server;};
		void			update_streamcount(size_t bytes) {_streamcount += bytes;};
		void			append_body(std::string &buffer);

		bool			is_ready() {return _ready;};

		ResponseType	get_type() {return _type;};
		int				get_status_code() const {return _status_code;};
		size_t			get_streamcount() const {return _streamcount;};
		std::string		get_status_mssg() const {return _status_message;};
		std::string		get_path() const {return _path;};
		std::string		get_body() const {return _body;};

		std::string		to_string() const;
		void 			set_error_response(const int status, const std::string &status_mssg);
		void			set_redirection(const int redirection_code, const std::string& message){_redirection = {redirection_code, message};};
		void 			insert_header(std::pair<std::string, std::string> key_value_pair){_header.insert(key_value_pair);};

	private:
		int				_status_code;
		size_t			_streamcount;
		std::string		_status_message;
		std::string		_virtual_host;
		std::string		_server;
		std::string		_body;
		std::string		_path;
		std::pair<int, std::string>	_redirection;
		std::unordered_map<std::string, std::string> _header;
		bool			_ready;
		ResponseType	_type;
};
