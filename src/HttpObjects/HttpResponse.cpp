#include "HttpResponse.hpp"
#include "Logger.hpp"
#include <cstdlib>

HttpResponse::HttpResponse() : _status_code(0), _streamcount(0), _ready(NOT_READY), _type(ResponseType::Unknown)
{


}

HttpResponse::~HttpResponse()
{

}

void 		HttpResponse::set_error_response(const int status, const std::string &status_mssg)
{
	_status_code = status;
	_status_message = status_mssg;
	_type = ResponseType::Error;
	set_body("\r\n<h1>" + std::to_string(_status_code) + " " + _status_message + "</h1>\r\n");
}

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

void HttpResponse::insert_header(std::pair<std::string, std::string> key_value_pair)
{
	auto it = _header.find(key_value_pair.first);
	if (it == _header.end())
	{
		_header.insert(key_value_pair);
		return;
	}
	else
	{
		_header.erase(it);
		_header.insert(key_value_pair);
	}
}

void			HttpResponse::append_body(std::string &buffer)
{
	_body += buffer;
}

std::string		HttpResponse::to_string() const
{
	std::string response;


	response += "HTTP/1.1 " + std::to_string(_status_code) + " " + _status_message + "\r\n";
	for (auto [key, value]: _header)
	{
		response += key + ": " + value + "\r\n";
	}
	response += "\r\n" + _body + "\r\n";
	// // TODO For debugging
	// if (_status_code == 500)
	// {
	// 	LOG_ERROR("Exit code 500, exiting program...");
	// 	exit(1);
	// }
	return response;
}
