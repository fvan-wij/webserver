#pragma once

class HttpServer
{
	public:
		HttpServer();
		~HttpServer();

		void		handle_request(int fd); //Reads request, parses, generates response
	private:
		// std::string	_request_buffer;
		// HttpObject	_object;

};
