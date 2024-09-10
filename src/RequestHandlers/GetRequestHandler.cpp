#include "GetRequestHandler.hpp"
#include "Logger.hpp"


HttpResponse	GetRequestHandler::handle_request(const HttpRequest &request)
{
	HttpResponse response;
	LOG_NOTICE("Handling GET request...\n" << request);
	response.set_status_code(200);
	response.set_status_mssg("OK");
	response.set_body("\r\n<h1>Fakka strijders</h1>\n<form action='' method='post'><button name='foo' value='Yeet'>Yeet</button></form>\r\n");
	return response;
}
