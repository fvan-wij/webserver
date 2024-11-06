#include "RequestHandler.hpp"
#include "HttpResponse.hpp"
#include "Utility.hpp"

#include <filesystem>
#include <fstream>
#include <algorithm>

RequestHandler::~RequestHandler()
{
	// LOG(RED << "Deleting requesthandler" << END);
}

static std::string generate_list(std::filesystem::path path)
{
	std::string list;

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		std::string file_name = entry.path().string().substr(entry.path().string().find_last_of('/'), entry.path().string().length());
		std::string uri = path.string().substr(path.string().find_last_of('/')) + file_name;
		std::string href_open = "<a href=\'" + uri + "\'>";
		std::string item = "<li>" + href_open + file_name  + "</a></li>";
		list += item;
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

static std::string generate_directory_listing(std::string_view path)
{
	std::string directory_list;
	directory_list.append("<div class=\"fileBlock\">" + generate_list(path) + "</div>");
	return directory_list;
}

std::optional<std::string> RequestHandler::retrieve_index_html(std::string_view path)
{
	constexpr auto 	READ_SIZE 	= std::size_t(1024);
	auto 			file_stream = std::ifstream(path.data(), std::ios::binary);
	auto 			out 		= std::string();

	if (not file_stream)
	{
		LOG_ERROR("Could not open file");
		return std::nullopt;
	}
	else
	{
		auto buf = std::string(READ_SIZE, '\0');
		while (file_stream.read(&buf[0], READ_SIZE))
		{
			out.append(buf, 0, file_stream.gcount());
		}
		out.append(buf, 0, file_stream.gcount());
	}
	if (not out.empty())
	{
		return out;
	}
	else
	{
		return std::nullopt;
	}
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
	response.set_type(ResponseType::Error);
	return response;
}

HttpResponse	RequestHandler::generate_successful_response(int status_code, std::string_view path, ResponseType type)
{
	HttpResponse response;
	response.set_status_code(status_code);
	response.set_status_mssg("OK");
	response.set_type(type);
	switch (type)
	{
		case ResponseType::Regular:
			{
				response.set_state(READY);
				std::optional<std::string> html = retrieve_index_html(path);
				bool autoindexing = true;
				if (html)
				{
					response.set_body("\r\n" + html.value() + "\r\n");
				}
				else if (autoindexing) // Temp flag, but should be retrieved from config
				{
					if (path.rfind(".html") != std::string::npos) // Strips off index.html path if cannot be retrieved
					{
						LOG_DEBUG("path .html found, path is: " << path);
						path = path.substr(0, path.find_last_of("/"));
						LOG_DEBUG("path substr is: " << path);
					}
					std::string directory_list = generate_directory_listing(path);
					response.set_body("\r\n" + directory_list + "\r\n");
				}
			}
			break;
		case ResponseType::Fetch:
			{
				response.set_state(NOT_READY);
				response.set_streamcount(0);
				response.set_path(path.data());
			}
			break;
		case ResponseType::Upload:
			{
				response.set_state(NOT_READY);
				response.set_body("\r\n<h1>File uploaded</h1><a href=\"/\" role=\"button\">Go back</a>\r\n");
			}
			break;
		case ResponseType::Delete:
			{
				response.set_state(READY);
				response.set_body("\r\n<h1>File deleted</h1><a href=\"/\" role=\"button\">Go back</a>\r\n");
			}
			break;
		case ResponseType::CGI:
			{
				response.set_state(NOT_READY);
				response.set_body("\r\n<h1>CGI data</h1>\r\n");
				response.set_path(path.data());
			}
			break;
		case ResponseType::Error:
			{
				response.set_state(READY);
				response.set_status_mssg("ERROR");
				response.set_body("\r\n<h1>ERROR</h1>\r\n");
			}
			break;
	}
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

bool RequestHandler::is_multipart_content(const HttpRequest &request)
{
	std::optional<std::string_view> content_type = request.get_value("Content-Type");
	if (!content_type)
	{
		return false;
	}
	if (content_type.value().find("multipart") != std::string::npos)
	{
		return true;
	}
	return false;
}


std::string RequestHandler::get_file_extension(std::string path)
{
	size_t	extension_pos = path.find_last_of(".");
	return std::string(&path[extension_pos]);
}

std::filesystem::path RequestHandler::build_path(std::optional<std::string> root, std::string_view uri, std::optional<std::string> index)
{
	std::filesystem::path p(".");
	p += root.value();
	LOG_NOTICE("p: " << p);
	p += uri;
	LOG_NOTICE("uri: " << uri << ", p /= uri: " << p);
	if (index)
	{
		p /= index.value();
	}
	return p;
}
