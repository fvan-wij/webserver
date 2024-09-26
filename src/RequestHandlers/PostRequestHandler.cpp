#include "PostRequestHandler.hpp"

static bool	upload_file(std::vector<char> buffer, std::string_view uri, t_config &config)
{
	std::string_view sv_buffer(buffer.data(), buffer.size());
	size_t pos = sv_buffer.find("filename=");
	std::string path;
	if (pos != std::string::npos)
	{
		size_t end = sv_buffer.find(10, pos);
		pos+=10;
		std::string filename(&sv_buffer[pos], (end - 2) - (pos));
		filename.push_back('\0');
		path += ".";
		path += config.root;
		path += uri.data();
		path += "/";
		path += filename.data();
		LOG_DEBUG(path);
		std::ofstream outfile(path, std::ios::binary);
		bool begin_trimmed = false;
		for (size_t i = 0; i < buffer.size(); i++)
		{
			if (!begin_trimmed && i > 4 && buffer[i - 3] == '\r' && buffer[i - 2] == '\n'&& buffer[i - 1] == '\r' && buffer[i] == '\n')
			{
				buffer.erase(buffer.begin(), buffer.begin() + i + 1);
				begin_trimmed = true;
			}
			if (begin_trimmed && i > 6 && buffer[i - 5] == '-' && buffer[i - 4] == '-'&& buffer[i - 3] == '-' && buffer[i - 2] == '-' && buffer[i - 1] == '-' && buffer[i] == '-')
			{
				buffer.erase(buffer.begin() + i - 5, buffer.end());
			}
		}
		outfile.write(buffer.data(), buffer.size());
		outfile.close();
		return true;
	}
	else 
		return false;
}

HttpResponse	PostRequestHandler::handle_request(const HttpRequest &request, t_config &config)
{
	LOG_NOTICE("Handling POST request\n" << request);

	if (!location_exists(config, request.get_uri()))
		return generate_error_response(404, "Not Found - The server cannot find the requested resource");
	if (!method_is_valid(request.get_uri(), request.get_method(), config))
		return generate_error_response(405, "Method Not Allowed - The request method is known by the server but is not supported by the target resource");
	if (content_length_exceeded(request, config))
		return generate_error_response(413, "Content Too Large");
	else if (request.get_uri() == "/cgi-bin")
		return generate_successful_response(200, "", ResponseType::CGI, config.root, request.get_location());
	else
	{
		std::string path = get_path(config.root, request.get_uri());
		path += config.location["/"].index;
		if (!upload_file(request.get_body(), request.get_uri(), config))
			LOG_ERROR("Couldn't upload file");
		return generate_successful_response(200, path, ResponseType::UPLOAD, config.root, request.get_location());
	}
	return generate_error_response(400, "Bad Request");
}
