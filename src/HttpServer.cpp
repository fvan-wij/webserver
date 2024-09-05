#include "HttpServer.hpp"
#include "meta.hpp"
#include <cwchar>
#include <string>


HttpServer::HttpServer(Socket &s) : _ready(false), _socket(s)
{

}

HttpServer::HttpServer(const HttpServer &other) : _request_buffer(other._request_buffer), _ready(other._ready), _socket(other._socket)
{
	// LOG("HttpServer : copied for sock_fd: " << _socket.get_fd());
}


void	HttpServer::handle(std::string data)
{
	_ready = false;
	HttpServer::set_request(data);
	// LOG("HttpServer : data [" << data << "]");
	UNUSED(data);
	// Append to `_request_buffer` until we reach EOF?
	// RUN CGI and other bullshit.
	// After CGI has exited (which we will check externally) we'll set _ready to true.

	if (HttpServer::object[int(Http::REQUEST)].trigger_cgi())
		_cgi.start("sleep_echo_var");
	else
	{
		HttpServer::set_response();
	}
}



std::string	HttpServer::get_data() const
{
	// if (!_ready)
	// {
	// 	WARNING("calling get_data() while not ready!");
	// }
	// std::string s = 
	// "HTTP/1.1 200 OK\r\n"
	// "\r\n<h1> Fakka strijders </h1>\r\n"
	// "\r\n";
	//
	// s += _cgi.get_buffer();
	return HttpServer::object[int(Http::RESPONSE)].get_body();
}

bool		HttpServer::is_ready()
{
	return _ready;
}

void 		HttpServer::poll_cgi()
{
	_ready = _cgi.poll();
}

HttpServer::~HttpServer()
{
	// LOG("HttpServer : destoryed | fd: " << _socket.get_fd());
}

void		HttpServer::set_request(std::string buffer)
{
	object[int(Http::REQUEST)] = HttpObject(buffer);
}

void HttpServer::set_response()
{
	std::string s = 
	"HTTP/1.1 200 OK\r\n"
	"\r\n<h1> Fakka strijders </h1>\r\n"
	"\r\n";

	HttpServer::object[int(Http::RESPONSE)].set_body(s);
}
