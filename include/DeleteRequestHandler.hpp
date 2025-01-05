#pragma once

#include "RequestHandler.hpp"

class DeleteRequestHandler : public RequestHandler
{
	public:
		HttpResponse	build_response(HttpRequest &request, Config &config, uint16_t port);
};
