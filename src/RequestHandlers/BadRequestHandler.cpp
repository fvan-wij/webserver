#include "BadRequestHandler.hpp"


HttpResponse	BadRequestHandler::handle_request(const HttpRequest &request)
{
	HttpResponse response;
	LOG("Handling BAD request...\n" << request);
	response.set_status_code(400);
	response.set_status_mssg("ERROR");
	return response;
}
