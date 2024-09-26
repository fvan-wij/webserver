#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <exception>
#include <optional>

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
		std::string										get_location() const {return _location;};
		std::string										get_filename() const {return _filename;};
		std::vector<char> 								get_body() const {return _body;};
		const std::string 								get_buffer() const {return _buffer;};
		std::unordered_map<std::string, std::string>	get_headers() const {return _header;};
		std::optional<std::string_view>					get_value(const std::string &key) const;
		RequestType										get_type() const {return _type;};
		bool											get_header_parsed() {return _b_header_parsed;};
		bool											get_body_parsed() {return _b_body_parsed;};
		bool											is_file() const {return _b_file;};

		void											set_type(RequestType type);
		void											set_header_parsed(bool state) {_b_header_parsed = state;};
		void											set_body_parsed(bool state) {_b_body_parsed = state;};
		void											set_body(std::vector<char> body) {_body = body;};
		void											append_buffer(std::string &data);

		void											parse_header(const std::string &data);
		void 											parse_body(std::vector<char> data);

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
		std::string	_buffer;
		int			_body_size;
		size_t		_body_index;
		bool		_b_header_parsed;
		bool		_b_body_parsed;
		bool		_b_file;
		std::string _method;
		std::string _protocol;
		std::string _uri;
		std::string _location;
		std::string _filename;
		std::vector<char> _body;
		void		_parse_request_line(std::istringstream 	&stream);
		std::unordered_map<std::string, std::string>	_header;
		RequestType	_type;
};

std::ostream & operator << (std::ostream &out, HttpRequest &request);
std::ostream & operator << (std::ostream &out, const HttpRequest &request);
