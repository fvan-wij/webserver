#include "GetRequestHandler.hpp"
#include "Logger.hpp"

HttpResponse	GetRequestHandler::handle_request(const HttpRequest &request, t_config &config)
{
	HttpResponse response;
	// LOG_NOTICE("Handling GET request...\n" << request);
	LOG_NOTICE("Handling GET request...\n");
	if (request.get_uri() == "/cgi-bin")
	{
		response.set_status_code(200);
		response.set_status_mssg("CGI data");
		std::string mssg = "\r\n<h1>" + std::to_string(response.get_status_code()) + " " + response.get_status_mssg() + "</h1>\r\n";
		response.set_body(mssg);
		response.set_state(NOT_READY);
		response.set_type(ResponseType::CGI);
		return response;
	}
	else if (validate_method(request, config))
	{
		std::string path = "." + config.root + request.get_uri();
		if (!config.location_map["/"].index.empty())
			path += config.location_map["/"].index;
		LOG_DEBUG(path);
		response.set_body("\r\n" + retrieve_html(path) + "\r\n");
		response.set_state(READY);
		response.set_type(ResponseType::REGULAR);
		return response;
	}
	else
	{
		response.set_status_code(400);
		response.set_status_mssg("Bad Request - Method not allowed");
		std::string mssg = "\r\n<h1>" + std::to_string(response.get_status_code()) + " " + response.get_status_mssg() + "</h1>\r\n";
		response.set_body(mssg);
		response.set_state(READY);
		response.set_type(ResponseType::ERROR);
		return response;
	}
}
