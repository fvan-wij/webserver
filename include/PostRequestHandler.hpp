#pragma once

#include "RequestHandler.hpp"
#include "CGI.hpp"

class PostRequestHandler : public RequestHandler
{
	public:
		HttpResponse	handle_request(const HttpRequest &request);
};
