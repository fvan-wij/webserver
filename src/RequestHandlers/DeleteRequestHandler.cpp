#include "DeleteRequestHandler.hpp"
#include "Logger.hpp"
#include <cstdio>
#include <filesystem>

HttpResponse	DeleteRequestHandler::handle_request(const HttpRequest &request, t_config &config)
{
	LOG_NOTICE("Handling DELETE request...\n" << request);

	std::filesystem::path directorypath = "/home/fvan-wij/Documents/Github_Projects/webserver" + config.root + request.get_uri();
	std::filesystem::path absPath = std::filesystem::absolute(directorypath);
	LOG_DEBUG("Relative Path: " + directorypath.string());
	LOG_DEBUG("Absolute Path: " + absPath.string());
	if (std::filesystem::exists(directorypath))
	{
		for (const auto& entry : std::filesystem::directory_iterator(directorypath)) { 
			std::cout << "File: " << entry.path() << std::endl; 
		} 
		std::string_view sv = std::string_view(request.get_uri()).substr(0, request.get_uri().find_last_of('/'));
		if (!location_exists(config, sv.data()))
			return generate_error_response(404, "Not Found - The server cannot find the requested resource");
		if (!method_is_valid(request.get_uri(), sv.data(), config))
			return generate_error_response(405, "Method Not Allowed - The request method is known by the server but is not supported by the target resource");
		std::string path;

		path += ".";
		path += get_path(config.root, request.get_uri());
		if (std::remove(path.c_str()) == 0)
			LOG_NOTICE("Successfully deleted " << path);

		return generate_successful_response(200, path, ResponseType::REGULAR);
	}
	return generate_error_response(400, "Bad Request");
}
