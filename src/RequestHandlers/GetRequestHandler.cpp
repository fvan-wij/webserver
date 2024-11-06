#include "GetRequestHandler.hpp"
#include "Logger.hpp"

HttpResponse	GetRequestHandler::handle_request(HttpRequest &request, t_config &config)
{
	LOG_NOTICE("Handling GET request:\n" << static_cast<const HttpRequest>(request));
	// LOG_NOTICE("Handling request: " << request.get_method() + " " + request.get_uri());

	if (!location_exists(config, request.get_location()))
		return generate_error_response(404, "Not Found - The server cannot find the requested resource");
	if (!method_is_valid(request.get_uri(), request.get_method(), config))
		return generate_error_response(405, "Method Not Allowed - The request method is known by the server but is not supported by the target resource");
	std::filesystem::path path = build_path(config.root, request.get_uri(), std::nullopt);
	if (std::filesystem::is_regular_file(path))
	{
		if (path.extension().string() == ".py")
			return generate_successful_response(200, path.string(), ResponseType::CGI);
		else
			return generate_successful_response(200, path.string(), ResponseType::Fetch);
	}
	else if (std::filesystem::is_directory(path))
	{
		path += config.location[request.get_location().data()].index;
		return generate_successful_response(200, path.string(), ResponseType::Regular);
	}
	else
		return generate_error_response(404, "Not Found - The server cannot find the requested resource");
}
