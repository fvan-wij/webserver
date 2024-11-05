#include "GetRequestHandler.hpp"
#include "Logger.hpp"

HttpResponse	GetRequestHandler::handle_request(const HttpRequest &request, Config &config)
{
	LOG_NOTICE("Handling GET request:\n" << request);
	// LOG_NOTICE("Handling request: " << request.get_method() + " " + request.get_uri());

	if (!location_exists(config, request.get_location()))
		return generate_error_response(404, "Not Found - The server cannot find the requested resource", config);
	if (!method_is_valid(request.get_uri(), request.get_method(), config))
		return generate_error_response(405, "Method Not Allowed - The request method is known by the server but is not supported by the target resource", config);
	std::string path = get_path(config.root, request.get_uri());
	if (request.is_file())
	{
		std::string ext = get_file_extension(request.get_uri());
		if (ext == ".py")
			return generate_successful_response(200, path, ResponseType::CGI);
		else
			return generate_successful_response(200, path, ResponseType::FETCH_FILE);
	}
	else
		path += config.location[request.get_location()].index;
	if (path.empty())
		return generate_error_response(404, "Not Found - The server cannot find the requested resource", config);
	return generate_successful_response(200, path, ResponseType::REGULAR);
}
