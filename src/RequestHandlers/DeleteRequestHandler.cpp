#include "DeleteRequestHandler.hpp"
#include "Logger.hpp"


HttpResponse	DeleteRequestHandler::handle_request(const HttpRequest &request)
{
	HttpResponse response;
	LOG_NOTICE("Handling DELETE request...\n" << request);
	response.set_status_code(200);
	response.set_status_mssg("OK");
	return response;
}
