#include "PostRequestHandler.hpp"

HttpResponse	PostRequestHandler::handle_request(const HttpRequest &request, t_config &config)
{
	LOG_NOTICE("Handling POST request\n" << request);
	// LOG_NOTICE("Handling request: " << request.get_method() + " " + request.get_uri());

	if (!location_exists(config, request.get_location()))
		return generate_error_response(404, "Not Found - The server cannot find the requested resource");
	if (!method_is_valid(request.get_location(), request.get_method(), config))
		return generate_error_response(405, "Method Not Allowed - The request method is known by the server but is not supported by the target resource");
	std::string path = get_path(config.root, request.get_uri());
	if (content_length_exceeded(request, config))
		return generate_error_response(413, "Content Too Large");
	else if (get_file_extension(request.get_uri()) == ".py")
		return generate_successful_response(200, path, ResponseType::CGI);
	else if (is_multipart_content(request))
		return generate_successful_response(200, path, ResponseType::UPLOAD);
	return generate_error_response(400, "Bad Request");
}
