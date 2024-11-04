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
		HttpRequest();
		~HttpRequest();
		std::string 									get_method() const;
		std::string 									get_uri() const;
		std::string 									get_protocol() const;
		const std::string 								get_buffer() const {return _buffer;};
		std::string										get_location() const {return _location;};
		std::string										get_filename() const {return _filename;};

		std::vector<char> 								get_body() const {return _body;};
		std::vector<char> 								get_body_buffer() const {return _body_buffer;};
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

		//												Parsing methods
		State											parse_header(std::vector<char>& data);
		State 											parse_body(std::vector<char>& data);
		void											parse_file_data(std::vector<char> buffer, std::string_view root, std::string_view uri);

		//												Data extraction methods
		void											extract_header_fields(std::string_view data_sv);
		std::string										extract_boundary(std::string_view content_type);
		std::string 									extract_file_path(std::string_view filename);
		std::string 									extract_filename(std::string_view body_buffer);

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
		size_t		_body_size;

		bool		_b_header_parsed;
		bool		_b_body_parsed;
		bool		_b_file;
		bool		_b_file_extracted;
		bool		_b_file_path_extracted;
		bool		_b_boundary_extracted;
		bool		_b_file_data_extracted;

		std::string	_buffer;
		std::string _method;
		std::string _protocol;
		std::string _uri;
		std::string _filename; // Uri filename
		std::string _location;
		std::string _boundary;
		std::string _boundary_end;


		FileUpload										_file;
		std::string										_header_buffer;
		std::vector<char> 								_body_buffer;
		std::unordered_map<std::string, std::string>	_header;
		std::vector<char> 								_body;

		RequestType	_type;

		void		_parse_request_line(std::istringstream 	&stream);
};

std::ostream & operator << (std::ostream &out, HttpRequest &request);
std::ostream & operator << (std::ostream &out, const HttpRequest &request);
