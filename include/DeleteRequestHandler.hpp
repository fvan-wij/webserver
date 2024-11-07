#pragma once

#include "RequestHandler.hpp"

class DeleteRequestHandler : public RequestHandler
{
	public:
		HttpResponse	handle_request(HttpRequest &request, Config &config);
};
