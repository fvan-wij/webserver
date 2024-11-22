#pragma once

#include "RequestHandler.hpp"

class GetRequestHandler : public RequestHandler
{
	public:
		HttpResponse	build_response(HttpRequest &request, Config &config);
};
