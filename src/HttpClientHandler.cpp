#include <HttpClientHandler.hpp>
#include <iostream>

HttpClientHandler::HttpClientHandler(ConnectionManager &cm, Socket socket, std::vector<Config> configs) : _configs(configs), _socket(socket), _connection_manager(cm)
{
}

void HttpClientHandler::handle_request()
{
	// _request = _socket.read();
	// _response = _request;
	// _socket.write(_response);
	std::cout << "Handling request" << std::endl;
}