#include "BadRequestHandler.hpp"
#include "Logger.hpp"


HttpResponse	BadRequestHandler::build_response(HttpRequest &request, Config &config, uint16_t port)
{
	HttpResponse response;
	(void) config;
	(void) port;
	LOG_NOTICE("Handling BAD request:\n" << static_cast<const HttpRequest>(request));

	if (request.get_type() == RequestType::Timeout)
	{
		response.set_status_code(408);
		response.set_status_mssg("Request Timeout");
		response.set_type(ResponseType::Error);
		response.set_body("\r\n<h1>" + std::to_string(response.get_status_code()) + " " + response.get_status_mssg() + "</h1>\r\n");
		return response;
	}
	response.set_status_code(400);
	response.set_status_mssg("Bad Request");
	response.set_type(ResponseType::Error);
	response.set_body("\r\n<h1>" + std::to_string(response.get_status_code()) + " " + response.get_status_mssg() + "</h1>\r\n");
	return response;
}
