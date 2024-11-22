#include "DeleteRequestHandler.hpp"
#include "Logger.hpp"
#include <cstdio>
#include <filesystem>

HttpResponse	DeleteRequestHandler::build_response(HttpRequest &request, Config &config)
{
	LOG_NOTICE("Handling DELETE request...\n" << static_cast<const HttpRequest>(request));
	// LOG_NOTICE("Handling request: " << request.get_method() + " " + request.get_uri());

	std::string uri = request.get_uri().data();
	std::filesystem::path directorypath = std::filesystem::current_path().string() + config.root + "/uploads";
	// std::filesystem::path filepath = std::filesystem::current_path().string() + config.root + uri;
	std::filesystem::path filepath = build_path(config.root, request.get_uri(), std::nullopt);
;
	LOG_DEBUG("filepath: " << filepath);

	if (std::filesystem::exists(filepath))
	{
		if (!location_exists(config, request.get_uri_as_path().parent_path().string()))
			return generate_error_response(404, "Not Found - Page not found", config, request);
		if (!method_is_valid(request.get_uri_as_path().parent_path().string(), request.get_method(), config))
			return generate_error_response(405, "Method Not Allowed - The request method is known by the server but is not supported by the target resource", config, request);
		if (std::remove(filepath.c_str()) == 0)
		{
			LOG_NOTICE("Successfully deleted " << filepath);
			return generate_successful_response(200, filepath.string(), ResponseType::Delete);
		}
	}
	return generate_error_response(400, "Bad Request", config, request);
}
