#include <ClientHandler.hpp>
#include "HandlerFactory.hpp"
#include <iostream>

/**
 * @brief ClientHandler; responsible for reading and sending data from and to the client. 
 * Acts as as finite-state machine based on its state (uploading, fetching, processingCGI).
 * Triggers timeouts when the timeout time is exceeded
 *
 * @param cm
 * @param socket
 * @param configs
 */
ClientHandler::ClientHandler(ConnectionManager &cm, Socket& socket, std::vector<Config>& configs) 
	: _configs(configs), _socket(socket), _connection_manager(cm), _file_handler(nullptr), _state(State::ParsingHeaders)
{

}

/**
 * @brief handles incoming data (POLLIN) and/or outgoing data (POLLOUT) based on the set 'events'
 *
 * @param events
 */
void ClientHandler::handle_request(short events)
{
	if (events & POLLIN)
	{
		_handle_incoming_data();
	}
	else if (events & POLLOUT)
	{
		_handle_outgoing_data();
	}
}

/**
 * @brief Reads and parses the incoming data;
 */
bool	ClientHandler::_handle_incoming_data()
{
	std::optional<std::vector<char>> incoming_data = _socket.read();
	if (not incoming_data)
	{
		LOG_ERROR("Failed to read request from client (fd " << _socket.get_fd() << ")");
		_connection_manager.remove(_socket.get_fd());
		return false;
	}
	else
	{
		LOG_INFO("Received request from client (fd " << _socket.get_fd() << ")" << " on server: " << _configs[0].server_name[0] << " on port: " << _socket.get_port());
		_parse(incoming_data.value());
		return true;
	}
}

/**
 * @brief Acts as a finite-state machine and processes, builds and sends data, based on its state
 */
void	ClientHandler::_handle_outgoing_data()
{
	switch(_state)
	{
		case State::Ready:
			_send_response(response.get_type());
			break;
		case State::FetchingFile:
			_poll_file_handler();
			break;
		case State::ProcessingRequest:
			_process_request();
			break;
		default:
			break;
	}
}

/**
 * @brief Parses the chunk of incoming data.
 * With each parsing call, the data is being moved to the request object
 * and repeated until the incoming data buffer is empty.
 *
 * @param data
 */
void	ClientHandler::_parse(std::vector<char>& data)
{
	while (not data.empty())
	{
		switch (_state)
		{
			case State::ParsingHeaders:
				{
					LOG_NOTICE("Parsing header...");
					_state = request.parse_header(data);
				}
				break;
			case State::ParsingBody:
				{
					LOG_NOTICE("Parsing body...");
					_state = request.parse_body(data);
				}
				break;
			default:
				break;
		}
	}
}

/**
 * @brief Sends a response based on the given type (Fetch, Upload, Delete, CGI)
 *
 * @param type
 */
bool	ClientHandler::_send_response(ResponseType type)
{
	if (type == ResponseType::Fetch)
	{
		std::vector<char>& data = _file_handler->get_file().data;
		if (not data.empty())
		{
			data.push_back('\0');
			response.set_body(data.data());
			_socket.write(response.to_string());
			_connection_manager.remove(_socket.get_fd());
			_connection_manager.remove(_file_handler->get_fd());
			return true;
		}
		else
			return false;
	}
	return false;
}

/**
 * @brief Polls the _file_handler and sets the finite-state machine to ready when the _file_handler is done
 */
void ClientHandler::_poll_file_handler()
{
	LOG_DEBUG("Waiting on FileHandler...");
	if (_file_handler->is_finished())
		_state = State::Ready;
}

/**
 * @brief Processes and validates the request. 
 * Creates a new event handler based on the ResponseType (Fetch, Upload, Delete, CGI)
 */
void	ClientHandler::_process_request()
{
	LOG_DEBUG("Processing request...");
	auto handler 	= HandlerFactory::create_handler(request.get_type());
	response 		= handler->handle_request(request, _configs[0]);
	if (response.get_type() == ResponseType::Fetch)
	{
		LOG_DEBUG("Creating FileHandler with file " << request.get_file().path);
		_file_handler = new FileHandler(request.get_file());
		short mask = POLLOUT;
		Action<FileHandler> *file_action = new Action<FileHandler>(_file_handler, &FileHandler::handle_file);
		_connection_manager.add(_file_handler->get_fd(), mask, file_action);
		_state = State::FetchingFile;
	}
}
