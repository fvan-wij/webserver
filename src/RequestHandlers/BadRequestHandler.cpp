#include "BadRequestHandler.hpp"
#include "Logger.hpp"


HttpResponse	BadRequestHandler::build_response(HttpRequest &request, Config &config)
{
	HttpResponse response;
	(void) config;
	(void) request;
	LOG_NOTICE("Handling BAD request:\n" << static_cast<const HttpRequest>(request));
	// LOG_NOTICE("Handling BAD request...");
	response.set_status_code(400);
	response.set_status_mssg("Bad Request");
	response.set_type(ResponseType::Error);
	response.set_body("\r\n<h1>" + std::to_string(response.get_status_code()) + " " + response.get_status_mssg() + "</h1>\r\n");
	return response;
}
