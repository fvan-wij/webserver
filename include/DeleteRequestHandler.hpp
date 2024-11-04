#pragma once

#include "RequestHandler.hpp"

class DeleteRequestHandler : public RequestHandler
{
	public:
		HttpResponse	handle_request(HttpRequest &request, t_config &config);
};
