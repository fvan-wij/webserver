#include "PostRequestHandler.hpp"
// #include <algorithm>

HttpResponse	PostRequestHandler::handle_request(const HttpRequest &request, t_config &config)
{
	// LOG_NOTICE("Handling POST request\n" << request);
	LOG_NOTICE("Handling request: " << request.get_method() + " " + request.get_uri());

	if (!location_exists(config, request.get_uri()))
		return generate_error_response(404, "Not Found - The server cannot find the requested resource");
	if (!method_is_valid(request.get_uri(), request.get_method(), config))
		return generate_error_response(405, "Method Not Allowed - The request method is known by the server but is not supported by the target resource");
	if (content_length_exceeded(request, config))
		return generate_error_response(413, "Content Too Large");
	else if (request.get_uri() == "/cgi-bin")
		return generate_successful_response(200, "", ResponseType::CGI);
	else if (is_multipart_content(request))
		return generate_successful_response(200, "", ResponseType::UPLOAD);
	return generate_error_response(400, "Bad Request");
}
