#include "PostRequestHandler.hpp"
// #include <algorithm>

static std::string get_file_path(std::string_view root, std::string_view uri, std::string_view filename)
{
	std::string path = ".";

	return path + root.data() + uri.data() + "/" + filename.data();
}

static std::string get_filename(std::string_view body_buffer)
{
	size_t filename_begin = body_buffer.find("filename=\"") + 10;
	size_t filename_end = body_buffer.find("\r\n", filename_begin);
	std::string filename (&body_buffer[filename_begin], &body_buffer[filename_end - 1]);
	LOG_DEBUG("FILENAME IS: " << filename);
	return filename;
}

static std::string get_boundary(std::string_view content_type)
{
	size_t boundary_begin = content_type.find("boundary=") + 9;
	std::string boundary(&content_type[boundary_begin], content_type.end());
	LOG_DEBUG("Boundary: " << boundary);
	return boundary;
}

static bool	upload_file(std::vector<char> buffer, std::string_view uri, t_config &config, std::string_view content_type)
{
	std::string_view 	sv_buffer(buffer.data(), buffer.size());
	std::string 		filename = get_filename(sv_buffer);
	std::string 		path = get_file_path(config.root, uri.data(), filename);
	std::string 		boundary_end = "--" + get_boundary(content_type) + "--";
	std::ofstream 		outfile(path, std::ios::binary);
	if (!filename.empty())
	{
		//Remove beginning till CRLN
		size_t crln_pos = sv_buffer.find("\r\n\r\n");
		buffer.erase(buffer.begin(), (buffer.begin() + crln_pos) + 4);

		//Remove ending boundary and everything after
		std::vector<char>::iterator it_boundary_end = std::search(buffer.begin(), buffer.end(), boundary_end.begin(), boundary_end.end());
		if (it_boundary_end != buffer.end())
		{
			buffer.erase(it_boundary_end - 2, buffer.end());
		}
		else
		{
			LOG_DEBUG("Not present!");
		}
		LOG_DEBUG("After\n" << buffer.data());
		outfile.write(buffer.data(), buffer.size());
		outfile.close();
		return true;
	}
	else 
		return false;
}

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
	else
	{
		//Upload file
		std::string path = get_path(config.root, request.get_uri());
		path += "/"; // ./var/www/uploads/
		path += config.location["/"].index; // html/index.html
		LOG_INFO("Path is: " << path);
		std::optional<std::string_view> content_type = request.get_value("Content-Type");
		if (content_type)
		{
			LOG_DEBUG("Content-Type: " << content_type.value());
			if (!upload_file(request.get_body(), request.get_uri(), config, content_type.value()))
				LOG_ERROR("Couldn't upload file");
		}
		else
		{
			LOG_ERROR("Couldn't upload file");
			return generate_error_response(400, "Bad Request");
		}
		return generate_successful_response(200, path, ResponseType::UPLOAD);
	}
	return generate_error_response(400, "Bad Request");
}
