#include "DeleteRequestHandler.hpp"
#include "Logger.hpp"
#include <cstdio>
#include <filesystem>

HttpResponse	DeleteRequestHandler::build_response(HttpRequest &request, Config &config, uint16_t port)
{
	LOG_NOTICE("Handling DELETE request...\n" << static_cast<const HttpRequest>(request));

	std::string uri = request.get_uri().data();
	std::filesystem::path filepath = build_path(config.root, request.get_uri(), std::nullopt);
	if (std::filesystem::exists(filepath))
	{
		if (!location_exists(config, request.get_uri_as_path().parent_path().string()))
			throw HttpException(404, "Not Found - Page not found");
		if (!method_is_valid(request.get_uri_as_path().parent_path().string(), request.get_method(), config))
			throw HttpException(405, "Method Not Allowed - The request method is known by the server but is not supported by the target resource");
		if (std::remove(filepath.c_str()) == 0)
		{
			LOG_NOTICE("Successfully deleted " << filepath);
			return generate_successful_response(port, filepath.string(), ResponseType::Delete);
		}
	}
	throw HttpException(400, "Bad Request");
}
