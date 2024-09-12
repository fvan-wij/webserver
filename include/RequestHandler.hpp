#pragma once
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Config.hpp"

#include <string_view>
#include <fstream>
#include <algorithm>

class RequestHandler {
	public:
		virtual HttpResponse	handle_request(const HttpRequest &request, t_config &config) = 0;
		std::string 			retrieve_html(std::string_view path);
		virtual ~RequestHandler();
};
