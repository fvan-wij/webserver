#include "PostRequestHandler.hpp"


HttpResponse	PostRequestHandler::handle_request(const HttpRequest &request)
{
	HttpResponse response;
	//Should trigger CGI
	response.set_status_code(200);
	response.set_status_mssg("OK");
	response.set_body("\r\n" + request.get_body() + "\r\n");
	return response;
}