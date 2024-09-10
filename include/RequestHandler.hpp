#pragma once
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class RequestHandler {
public:
	virtual HttpResponse	handle_request(const HttpRequest &request) = 0;
	virtual ~RequestHandler() = default;
};
