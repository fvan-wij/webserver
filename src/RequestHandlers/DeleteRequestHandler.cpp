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

		std::filesystem::perms permissions = std::filesystem::status(filepath).permissions();
		if ((permissions & std::filesystem::perms::owner_write) != std::filesystem::perms::none ||
				(permissions & std::filesystem::perms::group_write) != std::filesystem::perms::none ||
				(permissions & std::filesystem::perms::others_write) != std::filesystem::perms::none) {
			if (std::remove(filepath.c_str()) != 0)
			{
				LOG_ERROR("Error deleting file: " << filepath.string());
				throw HttpException(500, "Internal Server Error");
			}
			else
			{
				LOG_NOTICE("Successfully deleted " << filepath);
				return generate_successful_response(port, filepath.string(), ResponseType::Delete);
			}
		} 
		else
		{
			LOG_ERROR("You do not have permission to delete the file: " << filepath << '\n');
			throw HttpException(403, "Forbidden");
		}
	}
	throw HttpException(400, "Bad Request");
}
