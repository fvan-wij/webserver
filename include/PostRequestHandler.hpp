#pragma once

#include "RequestHandler.hpp"

class PostRequestHandler : public RequestHandler
{
	public:
		HttpResponse	handle_request(const HttpRequest &request, Config &config);
};
