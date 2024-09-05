#pragma once

#include "HttpObject.hpp"

class HttpRequest : public HttpObject {
public:
	HttpRequest();
	HttpRequest(HttpRequest &&) = default;
	HttpRequest(const HttpRequest &) = default;
	HttpRequest &operator=(HttpRequest &&) = default;
	HttpRequest &operator=(const HttpRequest &) = default;
	~HttpRequest();

	std::string 									&get_method();
	std::string 									&get_protocol();
	std::string 									&get_location();

private:
	
};

