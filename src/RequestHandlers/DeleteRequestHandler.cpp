#include "DeleteRequestHandler.hpp"
#include "Logger.hpp"

static bool			is_method_allowed(std::string_view method, std::vector<std::string> v)
{
	bool	allowed = false;

	if (v.size() > 0 && std::find(v.begin(), v.end(), method) != v.end())
		allowed = true;
	return allowed;
}

static bool	validate_method(const HttpRequest &request, t_config &config)
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

HttpResponse	DeleteRequestHandler::handle_request(const HttpRequest &request, t_config &config)
{
	(void) config;
	HttpResponse response;
	LOG_NOTICE("Handling DELETE request...\n" << request);
	response.set_status_code(200);
	response.set_status_mssg("OK");

	if (validate_method(request, config))
	{
		std::string path = "." + config.root + request.get_uri();
		if (!config.location_map["/"].index.empty())
			path += config.location_map["/"].index;
		LOG_DEBUG(path);
		response.set_body("\r\n" + retrieve_html(path) + "\r\n");

	}
	else
		response.set_body("\r\n<h1>Fakka strijders</h1>\n<form action='' method='post'><button name='foo' value='Yeet'>Yeet</button></form>\r\n");
	return response;
}
