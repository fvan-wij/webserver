#include "GetRequestHandler.hpp"
#include "Logger.hpp"

HttpResponse	GetRequestHandler::handle_request(const HttpRequest &request, t_config &config)
{
	HttpResponse response;
	LOG_NOTICE("Handling GET request...\n" << request);
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
	response.set_state(READY);
	response.set_type(ResponseType::REGULAR);
	return response;
}
