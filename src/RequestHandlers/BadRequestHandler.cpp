#include "BadRequestHandler.hpp"
#include "Logger.hpp"


HttpResponse	BadRequestHandler::handle_request(const HttpRequest &request)
{
	HttpResponse response;
	LOG_NOTICE("Handling BAD request...\n" << request);
	response.set_status_code(400);
	response.set_status_mssg("Bad Request");
	response.set_state(READY);
	response.set_type(ResponseType::ERROR);
	response.set_body("\r\n<h1>" + std::to_string(response.get_status_code()) + " " + response.get_status_mssg() + "</h1>");
	return response;
}
