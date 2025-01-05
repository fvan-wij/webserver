#include "GetRequestHandler.hpp"
#include "Logger.hpp"

bool contains_redirection(std::string_view loc, Config& config)
{
	auto it = config.location.find(loc.data());
	if (it != config.location.end() && config.location[loc.data()].redirection.first != 0)
		return true;
	return false;
}

HttpResponse	GetRequestHandler::build_response(HttpRequest &request, Config &config, uint16_t port)

{
	LOG_NOTICE("Handling GET request:\n" << static_cast<const HttpRequest>(request));

	std::filesystem::path path = build_path(config.root, request.get_uri(), std::nullopt);
	std::filesystem::path location(request.get_uri());
	if (std::filesystem::is_regular_file(path))
		location = location.parent_path();
	Location& location_block = config.location[location.string()];

	// Reflect with configuration
	if (!location_exists(config, location.string()))
		throw HttpException(404, "Not Found - The server cannot find the requested resource");
	if (!method_is_valid(location.string(), request.get_method(), config))
		throw HttpException(405, "Method Not Allowed - The request method is known by the server but is not supported by the target resource");
	if (contains_redirection(location.string(), config))
		throw HttpRedirection(location_block.redirection.first, location_block.redirection.second);

	//Determine if file or directory:
	if (std::filesystem::is_regular_file(path))
	{
		request.set_file_path(path.string());
		if (path.extension().string() == ".py")
			return generate_successful_response(port, path.string(), ResponseType::CGI);
		else
		{
			return generate_successful_response(port, path.string(), ResponseType::Fetch);
		}
	}
	else if (std::filesystem::is_directory(path))
	{
		std::string index = location_block.index;
		path /= index;
		request.set_file_path(path.string());
		if (not index.empty() && std::filesystem::exists(path))
			return generate_successful_response(port, path.string(), ResponseType::Fetch);
		else if (location_block.autoindex)
			return generate_successful_response(port, path.string(), ResponseType::Autoindex);
	}
	throw HttpException(404, "Not Found - The server cannot find the requested resource");
}
