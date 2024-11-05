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
		virtual HttpResponse	handle_request(const HttpRequest &request, Config &config) = 0;

		bool					method_is_allowed(std::string_view method, std::vector<std::string> v);
		bool					contains_body(const HttpRequest &request);
		bool					method_is_valid(std::string_view uri, std::string_view method, Config &config);
		bool					location_exists(Config &config, std::string_view loc);
		bool 					content_length_exceeded(const HttpRequest &request, Config &config);
		bool					is_multipart_content(const HttpRequest &request);

		std::string 				generate_error_body(int error_code, std::string_view message, Config &config);
		std::optional<std::string>	retrieve_error_path(int error_code, Config &config);
		HttpResponse			generate_error_response(int error_code, std::string_view message, Config &config);
		HttpResponse			generate_successful_response(int status_code, std::string_view path, ResponseType type);

		std::optional<std::string> 			retrieve_index_html(std::string_view path);
		std::string				get_path(std::string_view root, std::string_view uri);
		std::string 			get_file_extension(std::string path);
		virtual ~RequestHandler();
};
