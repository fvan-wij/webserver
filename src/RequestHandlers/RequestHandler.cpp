#include "RequestHandler.hpp"
#include "HttpResponse.hpp"

RequestHandler::~RequestHandler()
{
	// LOG(RED << "Deleting requesthandler" << END);
}

static std::string generate_list(std::filesystem::path directory)
{
	std::string list;

	for (const auto& entry : std::filesystem::directory_iterator(directory))
	{
		std::string file_path = "/uploads" + entry.path().string().substr(entry.path().string().find_last_of('/'), entry.path().string().length());// Fix this hardcoded shit!
		LOG_ERROR("generate list bla" << file_path);
		std::string img = "<li><a href=\"" + file_path + "\">" + file_path + "</a></li>";
		list += img;
	}
	if (list.empty())
		return {};
	else 
	{
		list.insert(0, "<ul>");
		list.append("</ul>");
		return list;
	}
}

static void insert_list_of_files(std::string &html, std::string_view root, std::string_view location)
{
	size_t insertion_index = html.find("<!--FILES-->");
	if (location == "/")
	{
		std::filesystem::path uploads = std::filesystem::current_path().string() + root.data() + "/uploads"; // Fix this hardcoded shit!
		LOG_ERROR("insert bla " << uploads);
		html.insert(insertion_index + 13, "<div class=\"fileBlock\">" + generate_list(uploads) + "</div>");
	}
}

std::string RequestHandler::retrieve_html(std::string_view path, std::string_view root, std::string_view location)
{
	constexpr auto 	read_size 	= std::size_t(1024);
	auto 			file_stream = std::ifstream(path.data(), std::ios::binary);
	auto 			out 		= std::string();

	LOG_DEBUG(path);
	if (not file_stream)
		LOG_ERROR("Could not open file");
	else
	{
		auto buf = std::string(read_size, '\0');
		while (file_stream.read(&buf[0], read_size))
		{
			out.append(buf, 0, file_stream.gcount());
		}
		out.append(buf, 0, file_stream.gcount());
		if (path.find("index.html") != std::string::npos)
		{
			insert_list_of_files(out, root, location);
		}
		return out;
	}
	return out;
}

bool			RequestHandler::method_is_allowed(std::string_view method, std::vector<std::string> v)
{
	bool	allowed = false;

	if (v.size() > 0 && std::find(v.begin(), v.end(), method) != v.end())
	{
		allowed = true;
	}
	return allowed;
}

bool	RequestHandler::method_is_valid(std::string_view uri, std::string_view method, t_config &config)
{
	t_location loc;
	auto it = config.location.find(uri.data());
	if (it != config.location.end())
	{
		loc = it->second; //Assign found location to loc;
		if (method_is_allowed(method, loc.allowed_methods))
		{
			return true;
		}
		else if (loc.allowed_methods.empty() && (config.methods.empty() || method_is_allowed(method, config.methods)))
		{
			return true;
		}
		else if (!method_is_allowed(method, loc.allowed_methods))
		{
			return false;
		}
	}
	else if (config.methods.empty() || method_is_allowed(method, config.methods))
	{
		return true;
	}
	return false;
}

bool RequestHandler::content_length_exceeded(const HttpRequest &request, t_config &config)
{
	std::optional<std::string_view> sv_conlen = request.get_value("Content-Length");
	if (sv_conlen)
	{
		if (Utility::svtoi(sv_conlen) > config.client_max_body_size)
		{
			LOG_ERROR("client max body exceeded");
			return true;
		}
	}
	return false;
}

HttpResponse	RequestHandler::generate_error_response(int error_code, std::string_view message)
{
	HttpResponse	response;

	response.set_status_code(error_code);
	response.set_status_mssg(message.data());
	std::string mssg = "\r\n<h1>" + std::to_string(response.get_status_code()) + " " + response.get_status_mssg() + "</h1>\r\n";
	response.set_body(mssg);
	response.set_state(READY);
	response.set_type(ResponseType::ERROR);
	return response;
}

HttpResponse	RequestHandler::generate_successful_response(int status_code, std::string_view path, ResponseType type, std::string_view root, std::string_view location)
{
	HttpResponse response;
	response.set_status_code(status_code);
	response.set_status_mssg("OK");
	if (type == ResponseType::REGULAR)
	{
		response.set_state(READY);
		response.set_body("\r\n" + retrieve_html(path, root, location) + "\r\n");
	}
	else if (type == ResponseType::AUTOINDEX)
	{
		response.set_state(READY);
		response.set_body("\r\n" + generate_directory_list_HTML(path) + "\r\n");
	}
	else if (type == ResponseType::UPLOAD)
	{
		response.set_state(READY);
		response.set_body("\r\n<h1>File uploaded</h1><a href=\"/\" role=\"button\">Go back</a>\r\n");
	}
	else if (type == ResponseType::DELETE)
	{
		response.set_state(READY);
		response.set_body("\r\n<h1>File deleted</h1>\r\n");
	}
	else if (type == ResponseType::CGI)
	{
		response.set_state(NOT_READY);
		response.set_body("\r\n<h1>CGI data</h1>\r\n");
	}
	response.set_type(type);
	return response;
}

std::string	RequestHandler::get_path(std::string_view root, std::string_view uri)
{
	std::string path = ".";
	path.append(root.data());
	path.append(uri.data());
	return path;
}

bool	RequestHandler::location_exists(t_config &config, std::string_view loc)
{
	auto it = config.location.find(loc.data());
	if (it != config.location.end())
		return true;
	return false;

}

std::string RequestHandler::generate_directory_list_HTML(std::filesystem::path directory)
{
	std::string list;

	for (const auto& entry : std::filesystem::directory_iterator(directory))
	{
		std::string img = "<li>" + entry.path().string() + "</li>";
		list += img;
	}
	if (list.empty())
		return {};
	else 
	{
		std::string html = HTML_TEMPLATE;
		list.insert(0, "<ul>");
		list.append("</ul>");
		html.insert(html.find("<!--CONTENT-->") + 14, list);
		return html;
	}
}

