#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <exception>

#include "meta.hpp"

class HttpRequest
{
public:
	std::string 									get_method() const;
	std::string 									get_uri() const;
	std::string 									get_protocol() const;
	const std::string 								get_body() const {return _body;};
	std::unordered_map<std::string, std::string>	get_headers() const {return _header;};
	std::string										get_value(const std::string &key) const;

	void											parse(const std::string &buffer);

	class HttpException : public std::exception
	{
		private:
			std::string	message;

		public:
			HttpException(const char* msg) : message(msg){};
			const char* what() const throw()
			{
				return message.c_str();
			}
	};

private:
	std::string _method;
	std::string _protocol;
	std::string _uri;
	std::string _body;
	void		_parse_request_line(std::istringstream 	&stream);
	std::unordered_map<std::string, std::string>	_header;
	
};

std::ostream & operator << (std::ostream &out, HttpRequest &request);
std::ostream & operator << (std::ostream &out, const HttpRequest &request);
