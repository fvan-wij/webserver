#pragma once
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Config.hpp"

#include <string_view>

class RequestHandler {
	public:
		virtual HttpResponse				build_response(HttpRequest &request, Config &config, uint16_t port) = 0;

		bool								method_is_allowed(std::string_view method, std::vector<std::string> v);
		bool								contains_body(const HttpRequest &request);
		bool								method_is_valid(std::string_view uri, std::string_view method, Config &config);
		bool								location_exists(Config &config, std::string_view loc);
		bool 								content_length_exceeded(const HttpRequest &request, Config &config);
		bool								is_multipart_content(const HttpRequest &request);

		std::optional<std::string>			retrieve_error_path(int error_code, Config &config);
		HttpResponse						generate_error_response(int error_code, std::string_view message, Config &config, HttpRequest& request);
		HttpResponse						generate_successful_response(uint16_t port, std::string_view path, ResponseType type);

		std::optional<std::string> 			retrieve_index_html(std::string_view path);
		std::string							get_path(std::string_view root, std::string_view uri);
		std::string 						get_file_extension(std::string path);
		std::filesystem::path 				build_path(std::optional<std::string> root, std::string_view uri, std::optional<std::string> index);
		virtual ~RequestHandler();
};
