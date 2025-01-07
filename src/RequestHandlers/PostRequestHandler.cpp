#include "PostRequestHandler.hpp"
#include "Utility.hpp"

HttpResponse	PostRequestHandler::build_response(HttpRequest &request, Config &config, uint16_t port)
{
	LOG_NOTICE("Handling POST request\n" << static_cast<const HttpRequest>(request));
	// LOG_NOTICE("Handling request: " << request.get_method() + " " + request.get_uri());

	std::filesystem::path path = build_path(config.root, request.get_uri(), std::nullopt);
	std::filesystem::path location(request.get_uri());
	if (std::filesystem::is_regular_file(path))
		location = location.parent_path();

	if (!location_exists(config, location.string()))
	{
		throw HttpException(404, "Not Found - The server cannot find the requested resource");
	}
	if (!method_is_valid(location.string(), request.get_method(), config))
	{
		throw HttpException(405, "Method Not Allowed - The request method is known by the server but is not supported by the target resource");
	}
	if (content_length_exceeded(request, config))
	{
		throw HttpException(413, "Content Too Large");
	}
	else if (request.get_uri_as_path().extension() == ".py")
	{
		return generate_successful_response(port, path.string(), ResponseType::CGI);
	}
	else if (is_multipart_content(request))
	{
		std::string_view len = request.get_value("Content-Length").value_or("0");
		if (Utility::svtoi(len).value_or(0) != static_cast<int>(request.get_body_buffer().size()))
		{
			throw HttpException(400, "Bad Request");
		}
		request.set_file_path(path.string());
		return generate_successful_response(port, request.get_file().name, ResponseType::Upload);
	}
	else if (request.get_value("Transfer-Encoding") == "chunked")
	{
		request.set_file_path(path.string());
		std::string filename = "file_id" + Utility::generate_random_string(6);
		path /= filename;
		request.set_file_name(filename);
		return generate_successful_response(port, filename, ResponseType::Upload);
	}
	throw HttpException(400, "Bad Request");
}
