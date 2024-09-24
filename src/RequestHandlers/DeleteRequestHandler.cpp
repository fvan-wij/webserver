#include "DeleteRequestHandler.hpp"
#include "Logger.hpp"

HttpResponse	DeleteRequestHandler::handle_request(const HttpRequest &request, t_config &config)
{
	(void) config;
	HttpResponse response;
	LOG_NOTICE("Handling DELETE request...\n" << request);
	response.set_status_code(200);
	response.set_status_mssg("OK");

	if (method_is_valid(request.get_uri(), request.get_method(), config))
	{
		std::string path = "." + config.root + request.get_uri();
		if (!config.location["/"].index.empty())
			path += config.location["/"].index;
		LOG_DEBUG(path);
		response.set_body("\r\n" + retrieve_html(path) + "\r\n");

	}
	else
		response.set_body("\r\n<h1>Fakka strijders</h1>\n<form action='' method='post'><button name='foo' value='Yeet'>Yeet</button></form>\r\n");
	return response;
}
