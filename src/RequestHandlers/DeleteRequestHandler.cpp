#include "DeleteRequestHandler.hpp"
#include "Logger.hpp"
#include <cstdio>
#include <filesystem>

HttpResponse	DeleteRequestHandler::handle_request(const HttpRequest &request, Config &config)
{
	// LOG_NOTICE("Handling DELETE request...\n" << request);
	LOG_NOTICE("Handling request: " << request.get_method() + " " + request.get_uri());

	std::string uri = request.get_uri();
	std::filesystem::path directorypath = std::filesystem::current_path().string() + config.root + "/uploads";
	std::filesystem::path filepath = std::filesystem::current_path().string() + config.root + uri;

	if (std::filesystem::exists(filepath.string()))
	{
		std::string sv_loc (uri.data(), request.get_uri().find_last_of('/'));
		if (!location_exists(config, sv_loc))
			return generate_error_response(404, "Not Found - Page not found");
		if (!method_is_valid(sv_loc, request.get_method(), config))
			return generate_error_response(405, "Method Not Allowed - The request method is known by the server but is not supported by the target resource");
		if (std::remove(filepath.c_str()) == 0)
			LOG_NOTICE("Successfully deleted " << filepath);
		return generate_successful_response(200, directorypath.string(), ResponseType::DELETE);
	}
	return generate_error_response(400, "Bad Request");
}
