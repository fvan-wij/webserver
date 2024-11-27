#pragma once

#include <string>

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

class HttpResponse 
{
	public:
		HttpResponse();
		~HttpResponse();
		void			set_status_code(const int status);
		void			set_status_mssg(const std::string &mssg);
		void			set_body(const std::string &body);
		void			set_state(bool state){_ready = state;};
		void			set_type(ResponseType type){_type = type;};
		void			set_path(std::string path){_path = path;};
		void			set_streamcount(size_t bytes) {_streamcount = bytes;};
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

	private:
		int				_status_code;
		size_t			_streamcount;
		std::string		_status_message;
		std::string		_body;
		std::string		_path;
		bool			_ready;
		ResponseType	_type;
};
