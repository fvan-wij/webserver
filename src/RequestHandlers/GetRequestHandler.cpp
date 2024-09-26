#include "GetRequestHandler.hpp"
#include "Logger.hpp"

HttpResponse	GetRequestHandler::handle_request(const HttpRequest &request, t_config &config)
{
	LOG_NOTICE("Handling GET request:\n" << request);
	if (!location_exists(config, request.get_location()))
		return generate_error_response(404, "Not Found - The server cannot find the requested resource");
	if (!method_is_valid(request.get_location(), request.get_method(), config))
		return generate_error_response(405, "Method Not Allowed - The request method is known by the server but is not supported by the target resource");
	std::string path = get_path(config.root, request.get_uri());
	if (!request.is_file())
	{
		path += config.location[request.get_location()].index;
		return generate_successful_response(200, path, ResponseType::REGULAR, config.root, request.get_location());
	}
	else
	{
		HttpResponse response;
		response.set_state(NOT_READY);
		response.set_type(ResponseType::FETCH_FILE);
		LOG_ERROR("Path: " << path);
		response.set_path(path);
		response.set_streamcount(0);
		return response;
	}
}
