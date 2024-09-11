#include "HttpServer.hpp"
#include "meta.hpp"
#include <cwchar>
#include <string>

HttpServer::HttpServer(Socket &s) : _socket(s)
{
	response.set_state(NOT_READY);
}

HttpServer::HttpServer(const HttpServer &other) : _request_buffer(other._request_buffer), _socket(other._socket)
{
	// LOG("HttpServer : copied for sock_fd: " << _socket.get_fd());
}

HttpServer &HttpServer::operator=(const HttpServer &rhs)
{
	if (this != &rhs)
	{
		_socket = rhs._socket;
		_request_buffer = rhs._request_buffer;
		// _cgi = rhs._cgi;
	}
	return *this;
	// LOG("HttpServer : copied for sock_fd: " << _socket.get_fd());
}

HttpServer::~HttpServer()
{
	// LOG(RED << "DELETING HTTPSERVER!" << END);
}

void	HttpServer::handle(HttpRequest &request)
{
	if (request.get_method() == "POST")
	{
		_cgi.start("sleep_echo_var");
		auto handler = HandlerFactory::create_handler(request.get_method());
		this->response = handler->handle_request(request);
		this->response.set_state(NOT_READY);
		this->response.set_type(ResponseType::CGI);
	}
	else 
	{
		auto handler = HandlerFactory::create_handler(request.get_method());
		this->response = handler->handle_request(request);
		this->response.set_state(READY);
		this->response.set_type(ResponseType::REGULAR);
	}
}

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

