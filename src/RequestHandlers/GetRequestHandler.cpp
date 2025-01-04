#include "GetRequestHandler.hpp"
#include "Logger.hpp"
#include "meta.hpp"

bool contains_redirection(std::string_view loc, Config& config)
{
	auto it = config.location.find(loc.data());
	if (it != config.location.end() && config.location[loc.data()].redirection.first != 0)
		return true;
	// LOG_DEBUG("Redirection status: " << config.location[loc.data()].redirection.first << ", ->: " << config.location[loc.data()].redirection.second);
	return false;
}

HttpResponse	GetRequestHandler::build_response(HttpRequest &request, Config &config)

{
	// LOG_NOTICE("Handling GET request:\n" << static_cast<const HttpRequest>(request));
	/*LOG_NOTICE("Handling GET request:\n");*/

	if (!location_exists(config, request.get_location()))
		throw HttpException(404, "Not Found - The server cannot find the requested resource");

	Location& location_block = config.location[request.get_location().data()];
	LOG_DEBUG("request.get_locatiion().data(): " << request.get_location().data());
	LOG_DEBUG("location_block.index: " << location_block.index);

	if (!method_is_valid(request.get_uri(), request.get_method(), config))
	{
		throw HttpException(405, "Method Not Allowed - The request method is known by the server but is not supported by the target resource");
	}

	if (contains_redirection(request.get_location(), config))
	{
		std::pair<int, std::string> redirection = location_block.redirection;
		//  throw HttpRedirection(redirection.first, REDIRECTION.at(redirection.first));
		LOG_DEBUG("NKAAKNEKNAEKNKER");
		throw HttpRedirection(redirection.first, redirection.second);
	}

	std::filesystem::path path = build_path(config.root, request.get_uri(), std::nullopt);
	if (std::filesystem::is_regular_file(path))
	{
		LOG_WARNING("ITS A REGULAR FILE??????????");
		request.set_file_path(path.string());
		if (path.extension().string() == ".py")
			return generate_successful_response(200, path.string(), ResponseType::CGI);
		else
		{
			return generate_successful_response(200, path.string(), ResponseType::Fetch);
		}
	}
	else if (std::filesystem::is_directory(path))
	{
		LOG_DEBUG("path: " << path.string());
		LOG_DEBUG("location.block: " << location_block.path);
		std::string index = location_block.index;
		path /= index;
		request.set_file_path(path.string());
		if (not index.empty() && std::filesystem::exists(path))
			return generate_successful_response(200, path.string(), ResponseType::Fetch);
		else if (location_block.autoindex)
			return generate_successful_response(200, path.string(), ResponseType::Autoindex);
	}
	throw HttpException(404, "Not Found - The server cannot find the requested resource");
}
