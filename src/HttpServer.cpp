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
	_ready = false;
	LOG_INFO("HttpServer : data [" << data << "]");
	LOG_INFO("HttpServer with sock_fd " << _socket.get_fd() <<" : received some  data");
	// Append to `_request_buffer` until we reach EOF?
	// RUN CGI and other bullshit.
	// After CGI has exited (which we will check externally) we'll set _ready to true.
	_cgi.start("sleep_echo_var");
}



std::string	HttpServer::get_data() const
{
	if (!_ready)
	{
		WARNING("calling get_data() while not ready!");
	}
	std::string s = 
	"HTTP/1.1 200 OK\r\n"
	"\r\n<h1> Fakka strijders </h1>\r\n"
	"\r\n";

	s += _cgi.get_buffer();


	return s;
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
