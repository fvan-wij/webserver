#include "HttpServer.hpp"
#include "meta.hpp"
#include <cwchar>
#include <string>



HttpServer::HttpServer(Socket &s) : _ready(false), _socket(s)
{
	// LOG("HttpServer : created for sock_fd: " << s.get_fd());
}

HttpServer::HttpServer(const HttpServer &other) : _request_buffer(other._request_buffer), _ready(other._ready), _socket(other._socket)
{
	// LOG("HttpServer : copied for sock_fd: " << _socket.get_fd());
}


void	HttpServer::handle(std::string data)
{
	// LOG("HttpServer : data [" << data << "]");
	UNUSED(data);
	LOG("HttpServer with sock_fd " << _socket.get_fd() <<" : received some  data");
	// Append to `_request_buffer` until we reach EOF?
	// RUN CGI and other bullshit.
	// After CGI has exited (which we will check externally) we'll set _ready to true.
	_ready = true;
}



std::string	HttpServer::get_data() const
{
	if (!_ready)
	{
		WARNING("calling get_data() while not ready!");
	}
	std::string s = 
	"HTTP/1.1 200 OK\r\n"
	"\r\n<h1> Fakka strijders </h1>\r\n";

	return s;
}

bool		HttpServer::is_ready() const
{
	return _ready;
}

HttpServer::~HttpServer()
{
	// LOG("HttpServer : destoryed | fd: " << _socket.get_fd());
}
