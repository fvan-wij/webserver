#pragma once

#include "RequestHandler.hpp"

class GetRequestHandler : public RequestHandler
{
	public:
		HttpResponse	handle_request(const HttpRequest &request, t_config &config);
};
