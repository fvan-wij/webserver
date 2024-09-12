#include "HttpServer.hpp"
#include "meta.hpp"
#include <cwchar>
#include <string>

HttpServer::HttpServer()
{
	response.set_state(NOT_READY);
}

HttpServer::~HttpServer()
{
	// LOG(RED << "DELETING HTTPSERVER!" << END);
}

HttpServer::HttpServer(const HttpServer &other) : _request_buffer(other._request_buffer) 
{
	// LOG("HttpServer : copied for sock_fd: " << _socket.get_fd());
}


void	HttpServer::handle(HttpRequest &request)
{
	auto handler = HandlerFactory::create_handler(request.get_type());
	t_config config;
	response = handler->handle_request(request, config);
	if (response.get_type() == ResponseType::CGI)
		_cgi.start("sleep_echo_var");
}

// bool	HttpServer::is_request_valid(HttpRequest &request, t_config &config)
// {
// 	bool		permission_method = false;
// 	bool		permission_uri = false;
//
// 	for (auto methods : config.location.allowed_methods)
// 	{
// 		if (request.get_method() == methods)
// 			permission_method = true;
// 	}
//
// 	for (auto uris : config.location.URIs)
// 	{
// 		if (request.get_uri() == uris)
// 			permission_uri = true;
// 	}
// 	LOG_DEBUG(permission_method << ", " << permission_uri);
// 	if (!permission_method || !permission_uri)
// 	{
// 		request.set_type(RequestType::BadRequest);
// 		return false;
// 	}
// 	return true;
// }

std::string	HttpServer::get_data()
{
	if (!response.is_ready())
	{
		WARNING("calling get_data() while not ready!");
	}
	if (response.get_type() == ResponseType::CGI)
	{
		std::string b = _cgi.get_buffer();
		response.append_body(b);
	}
	return response.to_string();
}

bool		HttpServer::is_ready()
{
	return this->response.is_ready();
}

void 		HttpServer::poll_cgi()
{
	if (response.get_type() == ResponseType::REGULAR)
		return;
	else
		response.set_state(_cgi.poll());
}

