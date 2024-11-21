#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <exception>
#include <optional>
#include <filesystem>

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
		//												Getters
		std::string_view 								get_method() const;
		std::string_view								get_uri() const;
		std::filesystem::path							get_uri_as_path() const;
		std::string_view								get_protocol() const;
		std::string_view								get_location() const {return _location;};
		std::string_view								get_filename() const {return _filename;};
		RequestType										get_type() const {return _type;};
		std::vector<char>& 								get_body_buffer() {return _body_buffer;};
		std::optional<std::string_view>					get_value(const std::string &key) const;
		std::unordered_map<std::string, std::string>	get_headers() const {return _header;};
		File&											get_file() {return _file;};

		//												Bools
		bool											is_header_parsed() {return _b_header_parsed;};
		bool											is_body_parsed() {return _b_body_parsed;};
		bool											is_file() const {return _b_file;};

		//												Setters
		void											set_type(RequestType type);
		void											set_header_parsed(bool state) {_b_header_parsed = state;};
		void											set_body_parsed(bool state) {_b_body_parsed = state;};
		void 											set_file_path(std::string_view root);
		void											append_buffer(std::string &buffer);

		//												Parsing methods
		State											parse_header(std::vector<char>& buffer);
		State 											parse_body(std::vector<char>& buffer);

		//												Exceptions
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
		bool											_b_header_parsed;
		bool											_b_body_parsed;
		bool											_b_file;
		bool											_b_file_extracted;
		bool											_b_file_path_extracted;
		bool											_b_boundary_extracted;
		bool											_b_file_data_extracted;

		std::string 									_method;
		std::string 									_protocol;
		std::string 									_uri;
		std::string 									_filename;
		std::string 									_location;
		std::string 									_boundary;
		std::string 									_boundary_end;


		std::string										_header_buffer;
		std::vector<char> 								_body_buffer;
		std::unordered_map<std::string, std::string>	_header;

		RequestType										_type;
		File											_file;

		//												Data extraction methods
		void											_extract_request_line(std::istringstream 	&stream);
		void											_extract_header_fields(std::string_view data_sv);
		std::string										_extract_boundary(std::string_view content_type);
		std::string 									_extract_filename(std::string_view body_buffer);

};

//Insertion overloading
std::ostream & operator << (std::ostream &out, HttpRequest &request);
std::ostream & operator << (std::ostream &out, const HttpRequest &request);
