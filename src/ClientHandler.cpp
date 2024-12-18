#include <ClientHandler.hpp>
// #include <HandlerFactory.hpp>
#include <iostream>

/**
 * @brief ClientHandler; responsible for reading and sending data from and to the client.
 * Acts as as finite-state machine based on its state (uploading, fetching, processingCGI).
 * Triggers timeouts when the timeout time is exceeded
 *
 * @param cm: ConnectionManager object
 * @param socket: Socket object
 * @param configs: vector of configs
 */
ClientHandler::ClientHandler(ConnectionManager &cm, Socket socket, std::vector<Config>& configs)
	: _configs(configs), _socket(socket), _connection_manager(cm), _file_handler(nullptr), _state(State::ParsingHeaders), _timed_out(false)

{

}



/**
 * @brief handles incoming data (POLLIN) and/or outgoing data (POLLOUT) based on the set 'events'
 *
 * @param events: POLLIN and/or POLLOUT
 */
void ClientHandler::handle_event(short events)
{
	if (events & POLLIN)
	{
		_event_pollin();
	}
	if (events & POLLOUT)
	{
		_event_pollout();
	}
}

void	ClientHandler::_event_pollin()
{
	std::optional<std::vector<char>> raw_request = std::nullopt;
	raw_request = _socket.read();
	request_buffer_.insert(request_buffer_.end(), raw_request.value().begin(), raw_request.value().end());

	if (not raw_request)
	{
		LOG_ERROR("Failed to read request from client (fd " << _socket.get_fd() << ") Closing connection!");
		_close_connection();
	}
	if (!request.parse(raw_request.value()))
	{
		LOG_ERROR("Failed to parse request from client (fd " << _socket.get_fd() << ") Closing connection!");
		_close_connection();
	}

}

void	ClientHandler::_event_pollout()
{
	// send a defualt response for the time being
	_socket.write("HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n");
	_close_connection();
}

/**
 * @brief Removes and closes the client and/or filehandler connection
 */
void	ClientHandler::_close_connection()
{
	// if (_file_handler != nullptr)
	// 	_connection_manager.remove(_file_handler->get_fd());
	_connection_manager.remove(_socket.get_fd());
}