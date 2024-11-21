#include "PostRequestHandler.hpp"
#include "Utility.hpp"

HttpResponse	PostRequestHandler::handle_request(HttpRequest &request, Config &config)
{
	LOG_NOTICE("Handling POST request\n" << static_cast<const HttpRequest>(request));
	// LOG_NOTICE("Handling request: " << request.get_method() + " " + request.get_uri());

	if (!location_exists(config, request.get_location()))
	{
		return generate_error_response(404, "Not Found - The server cannot find the requested resource", config, request);
	}
	if (!method_is_valid(request.get_location(), request.get_method(), config))
	{
		return generate_error_response(405, "Method Not Allowed - The request method is known by the server but is not supported by the target resource", config, request);
	}
	std::filesystem::path path = build_path(config.root, request.get_uri(), std::nullopt);
	if (content_length_exceeded(request, config))
	{
		return generate_error_response(413, "Content Too Large", config, request);
	}
	else if (request.get_uri_as_path().extension() == ".py")
	{
		return generate_successful_response(200, path.string(), ResponseType::CGI);
	}
	else if (is_multipart_content(request))
	{
		std::string_view len = request.get_value("Content-Length").value_or("0");
		if (Utility::svtoi(len).value_or(0) != static_cast<int>(request.get_body_buffer().size()))
		{
			return generate_error_response(400, "Bad Request", config, request);
		}
		request.set_file_path(path.string());
		return generate_successful_response(200, request.get_file().name, ResponseType::Upload);
	}
	return generate_error_response(400, "Bad Request", config, request);
}
