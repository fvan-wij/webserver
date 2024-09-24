#include "GetRequestHandler.hpp"
#include "Logger.hpp"

HttpResponse	GetRequestHandler::handle_request(const HttpRequest &request, t_config &config)
{
	HttpResponse response;
	LOG_NOTICE("Handling GET request:\n" << request);
	//Check if method is allowed
	//Check if there's a body and/or Content-Length key-value pair
	//	This should not be present in a GET request, either ignore or generate appropriate error response
	//Get root location
	//	What if there's no root location? Perhaps this needs to be checked before setting up the server
	//Generate path to retrieve from, check if exists
	//Retrieve HTML
	//	If path to HTML does not exist: error!


	if (!method_is_valid(request.get_uri(), request.get_method(), config))
		return generate_error_response(400, "Bad Request - Method not allowed");
	if (contains_body(request))
		return generate_error_response(400, "Bad Request - GET request contains body");

	std::string path = "." + config.root + request.get_uri();
	if (!config.location["/"].index.empty())
		path += config.location["/"].index;
	LOG_DEBUG(path);
	response.set_body("\r\n" + retrieve_html(path) + "\r\n");
	response.set_state(READY);
	response.set_type(ResponseType::REGULAR);
	return response;
}
