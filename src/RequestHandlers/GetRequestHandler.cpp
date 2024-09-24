#include "GetRequestHandler.hpp"
#include "Logger.hpp"

HttpResponse	GetRequestHandler::handle_request(const HttpRequest &request, t_config &config)
{
	LOG_NOTICE("Handling GET request:\n" << request);

	if (!location_exists(config, request.get_uri()))
		return generate_error_response(404, "Not Found - The server cannot find the requested resource");
	if (!method_is_valid(request.get_uri(), request.get_method(), config))
		return generate_error_response(405, "Method Not Allowed - The request method is known by the server but is not supported by the target resource");

	std::string path = get_path(config.root, request.get_uri());
	path += config.location["/"].index;

	return generate_successful_response(200, path, ResponseType::REGULAR);
}
