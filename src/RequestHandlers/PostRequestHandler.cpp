#include "PostRequestHandler.hpp"

HttpResponse	PostRequestHandler::handle_request(HttpRequest &request, Config &config)
{
	LOG_NOTICE("Handling POST request\n" << static_cast<const HttpRequest>(request));
	// LOG_NOTICE("Handling request: " << request.get_method() + " " + request.get_uri());

	if (!location_exists(config, request.get_location()))
		return generate_error_response(404, "Not Found - The server cannot find the requested resource", config);
	if (!method_is_valid(request.get_location(), request.get_method(), config))
		return generate_error_response(405, "Method Not Allowed - The request method is known by the server but is not supported by the target resource", config);
	std::filesystem::path path = build_path(config.root, request.get_uri(), std::nullopt);
	if (content_length_exceeded(request, config))
		return generate_error_response(413, "Content Too Large", config);
	else if (request.get_uri_as_path().extension() == ".py")
		return generate_successful_response(200, path.string(), ResponseType::CGI);
	else if (is_multipart_content(request))
	{
		request.set_file_upload_path(path.string());
		return generate_successful_response(200, path.string(), ResponseType::Upload);
	}
	return generate_error_response(400, "Bad Request", config);
}
