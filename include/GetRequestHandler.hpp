#pragma once

#include "RequestHandler.hpp"
#include <string_view>

class GetRequestHandler : public RequestHandler
{

	public:
		HttpResponse	handle_request(const HttpRequest &request);
		std::string 	retrieve_html(std::string_view path);
};
