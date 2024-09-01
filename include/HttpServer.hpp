#pragma once
#include <vector>

class HttpServer
{
	public:
		HttpServer();
		~HttpServer();

		void		handle_request(int fd); //Reads request, parses, generates response
	private:
		// std::string	_request_buffer;
		// std::vector<HttpObject>	_object;

};
