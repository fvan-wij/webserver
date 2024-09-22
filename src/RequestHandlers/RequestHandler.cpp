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

	//This is all sooo scuffed and redundant, I know :P
	if (!config.methods.empty() && is_method_allowed(request.get_method(), config.methods)) //Global methods present but no methods present in local directive
	{
		t_location loc;
		auto it = config.location.find(request.get_uri());
		if (it != config.location.end())
		{
			loc = it->second;
			if (loc.allowed_methods.empty())
				return true;
		}
	}
	else if (config.methods.empty() || is_method_allowed(request.get_method(), config.methods)) //Global methods not present or global method allowed and local directive allowed
	{
		if (config.location.find(request.get_uri()) != config.location.end() && is_method_allowed(request.get_method(), config.location[request.get_uri()].allowed_methods))
		{
			return true;
		}
	}
	else if (config.location.find(request.get_uri()) != config.location.end() && is_method_allowed(request.get_method(), config.location[request.get_uri()].allowed_methods)) //Method is allowed in local directive
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
