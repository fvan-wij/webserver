#include "HttpResponse.hpp"
#include "Logger.hpp"
#include <system_error>

void		HttpResponse::set_status_code(const int status)
{
	_status_code = status;

}
void		HttpResponse::set_status_mssg(const std::string &mssg)
{
	_status_message = mssg;

}
void		HttpResponse::set_body(const std::string &body)
{
	_body = body;
}

void			HttpResponse::append_body(std::string &buffer)
{
	_body += buffer;
}

bool			HttpResponse::is_ready()
{
	// LOG_DEBUG("http reponse ready? | " << _is_ready);
	return	_is_ready;
}

void HttpResponse::set_state(bool state)
{
	_is_ready = state;
}

std::string		HttpResponse::to_string() const
{
	std::string response;
	response += "HTTP/1.1 " + std::to_string(_status_code) + " " + _status_message + "\r\n";
	response += _body + "\r\n";
	return response;
}
