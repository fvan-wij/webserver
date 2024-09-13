#include "PostRequestHandler.hpp"

HttpResponse	PostRequestHandler::handle_request(const HttpRequest &request, t_config &config)
{
	(void) config;
	HttpResponse response;
	//Should trigger CGI
	LOG_NOTICE("Handling POST request...\n" << request);
	response.set_status_code(200);
	response.set_status_mssg("OK");
	if (request.get_uri() == "/cgi-bin")
	{
		response.set_state(NOT_READY);
		response.set_type(ResponseType::CGI);
		std::string mssg = "<h1>CGI data</h1>";
		response.set_body("\r\n" + mssg + "\r\n");
	}
	else
	{
		std::string mssg = "<h1>Uploading file</h1>";
		response.set_body("\r\n" + mssg + "\r\n");
		response.set_state(READY);
		response.set_type(ResponseType::REGULAR);
	}
	return response;
}
