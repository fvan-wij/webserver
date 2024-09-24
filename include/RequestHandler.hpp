#pragma once
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Config.hpp"
#include "Utility.hpp"

#include <string_view>
#include <fstream>
#include <algorithm>

class RequestHandler {
	public:
		virtual HttpResponse	handle_request(const HttpRequest &request, t_config &config) = 0;
		std::string 			retrieve_html(std::string_view path);
		bool					method_is_allowed(std::string_view method, std::vector<std::string> v);
		// bool					validate_method(const HttpRequest &request, t_config &config);
		bool					contains_body(const HttpRequest &request);
		bool					method_is_valid(std::string_view uri, std::string_view method, t_config &config);
		bool 					content_length_exceeded(const HttpRequest &request, t_config &config);
		HttpResponse			generate_error_response(int error_code, std::string_view message);
		virtual ~RequestHandler();
};
