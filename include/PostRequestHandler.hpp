#pragma once

#include "RequestHandler.hpp"

class PostRequestHandler : public RequestHandler
{
	public:
		HttpResponse	build_response(HttpRequest &request, Config &config, uint16_t port);
};
