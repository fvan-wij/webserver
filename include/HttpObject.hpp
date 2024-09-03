#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <map>

enum class RequestType{
	UNINITIALIZED,
	POST,
	GET,
	DELETE,
	CGI
};

class HttpObject {
public:
	HttpObject(const std::string &buffer);
	HttpObject(HttpObject &&) = default;
	HttpObject(const HttpObject &) = default;
	HttpObject &operator=(HttpObject &&) = default;
	HttpObject &operator=(const HttpObject &) = default;
	~HttpObject();

	std::string							&get_body();
	std::map<std::string, std::string>	&get_header();
	int32_t								get_content_length();

private:
	std::map<std::string, std::string>	_header;
	std::string							_body;
	RequestType							_type;
};

std::ostream & operator << (std::ostream &out, HttpObject &obj);
