#pragma once

#include <string>
#include "meta.hpp"

enum {
	NOT_READY = false,
	READY = true,
};

enum class ResponseType:int {
	REGULAR = 0,
	UPLOAD = 1,
	CGI = 2,
};

class HttpResponse 
{
	public:
		void			set_status_code(const int status);
		void			set_status_mssg(const std::string &mssg);
		void			set_body(const std::string &body);
		void			set_state(bool state){_ready = state;};
		void			set_type(ResponseType type){_type = type;};

		void			append_body(std::string &buffer);

		bool			is_ready(){return _ready;};

		ResponseType	get_type(){return _type;};

		std::string		to_string() const;

	private:
		int				_status_code;
		std::string		_status_message;
		std::string		_body;
		bool			_ready;
		ResponseType	_type;
};
