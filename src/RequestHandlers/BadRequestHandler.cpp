#include "BadRequestHandler.hpp"
#include "Logger.hpp"


HttpResponse	BadRequestHandler::handle_request(const HttpRequest &request)
{
	HttpResponse response;
	LOG_ERROR("Handling BAD request...\n" << request);
	response.set_status_code(400);
	response.set_status_mssg("ERROR");
	return response;
}
