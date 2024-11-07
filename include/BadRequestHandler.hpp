#pragma once

#include "RequestHandler.hpp"

class BadRequestHandler : public RequestHandler
{
	public:
		HttpResponse	handle_request(HttpRequest &request, Config &config);
};
