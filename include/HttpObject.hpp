#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <unordered_map>

#include "meta.hpp"

#define RED 	"\x1B[31m"
#define GREEN 	"\x1B[32m"
#define YELLOW 	"\x1B[33m"
#define END		"\033[0m\t\t"

class HttpObject {
public:
	HttpObject(const std::string &buffer);
	HttpObject(HttpObject &&) = default;
	HttpObject(const HttpObject &) = default;
	HttpObject &operator=(HttpObject &&) = default;
	HttpObject &operator=(const HttpObject &) = default;
	~HttpObject();

	std::string										&get_body();
	std::unordered_map<std::string, std::string>	&get_header();
	std::string										&get_value(const std::string &key);
	std::string 									&get_method();
	std::string 									&get_protocol();
	std::string 									&get_location();

private:
	std::unordered_map<std::string, std::string>	_header;
	std::string										_body;
	std::string										_method;
	std::string										_protocol;
	std::string										_location;

};

std::ostream & operator << (std::ostream &out, HttpObject &obj);
