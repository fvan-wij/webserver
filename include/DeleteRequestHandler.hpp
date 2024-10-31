#pragma once

#include "RequestHandler.hpp"

class DeleteRequestHandler : public RequestHandler
{
	public:
		HttpResponse	handle_request(const HttpRequest &request, Config &config);
};
