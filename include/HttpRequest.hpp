#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <exception>

#include "meta.hpp"

enum class RequestType:int
{
	PostRequest,
	GetRequest,
	DeleteRequest,
	BadRequest,
};

const static std::unordered_map<std::string, RequestType> REQUEST_TYPE = 
{
	{"GET", RequestType::GetRequest},
	{"POST", RequestType::PostRequest},
	{"DELETE", RequestType::DeleteRequest},
};

class HttpRequest
{
	public:
		std::string 									get_method() const;
		std::string 									get_uri() const;
		std::string 									get_protocol() const;
		const std::string 								get_body() const {return _body;};
		std::unordered_map<std::string, std::string>	get_headers() const {return _header;};
		std::string										get_value(const std::string &key) const;
		RequestType										get_type() const {return _type;};

		void											parse(const std::string &buffer);

		void											validate_with_config();
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
		RequestType	_type;
};

std::ostream & operator << (std::ostream &out, HttpRequest &request);
std::ostream & operator << (std::ostream &out, const HttpRequest &request);
