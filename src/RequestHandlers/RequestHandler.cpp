#include "RequestHandler.hpp"

RequestHandler::~RequestHandler()
{
	// LOG(RED << "Deleting requesthandler" << END);
}

std::string RequestHandler::retrieve_html(std::string_view path)
{
	constexpr auto 	read_size 	= std::size_t(1024);
	auto 			file_stream = std::ifstream(path.data(), std::ios::binary);
	auto 			out 		= std::string();

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
		return out;
	}
	return out;
}

bool			RequestHandler::is_method_allowed(std::string_view method, std::vector<std::string> v)
{
	bool	allowed = false;

	if (v.size() > 0 && std::find(v.begin(), v.end(), method) != v.end())
		allowed = true;
	return allowed;
}

bool	RequestHandler::validate_method(const HttpRequest &request, t_config &config)
{
	if (config.methods.empty() || is_method_allowed(request.get_method(), config.methods))
	{
		if (config.location_map.find(request.get_uri()) != config.location_map.end() && is_method_allowed(request.get_method(), config.location_map[request.get_uri()].allowed_methods))
		{
			return true;
		}
	}
	else if (config.location_map.find(request.get_uri()) != config.location_map.end() && is_method_allowed(request.get_method(), config.location_map[request.get_uri()].allowed_methods))
	{
		return true;
	}
	return false;
}

