#include <ClientHandler.hpp>
#include "HandlerFactory.hpp"
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
	: _configs(configs), _socket(socket), _connection_manager(cm), _file_handler(nullptr), _state(State::ParsingHeaders)
{

}

/**
 * @brief handles incoming data (POLLIN) and/or outgoing data (POLLOUT) based on the set 'events'
 *
 * @param events: POLLIN and/or POLLOUT
 */
void ClientHandler::handle_request(short events)
{
	if (_is_timeout())
	{
		LOG_ERROR("Client on fd " << _socket.get_fd() << " timed out");
		_close_connection();
	}
	else if (events & POLLIN)
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
		case State::UploadingFile:
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
 * @param data: vector<char> read from client
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
 * @brief Sends a response based on the given type
 *
 * @param type: ResponseType::(Fetch, Upload, Delete, CGI)
 */
void	ClientHandler::_send_response(ResponseType type)
{
	if (type == ResponseType::Fetch || type == ResponseType::Error)
	{
		std::vector<char>& data = _file_handler->get_file().data;
		if (not data.empty())
		{
			data.push_back('\0');
			response.set_body(data.data());
			_socket.write(response.to_string());
			LOG_NOTICE("Response sent to fd=" << _socket.get_fd());
			_close_connection();
		}
	}
	else
	{
		_socket.write(response.to_string());
		LOG_NOTICE("Response sent to fd=" << _socket.get_fd());
		_close_connection();
	}
}

/**
 * @brief Polls the _file_handler and sets the finite-state machine to ready when the _file_handler is done
 */
void ClientHandler::_poll_file_handler()
{
	LOG_NOTICE("Waiting on FileHandler...");

	if (_file_handler->is_finished())
	{
		LOG_NOTICE("FileHandler is finished");
		_state = State::Ready;
	}
}

Config	ClientHandler::_resolve_config(std::optional<std::string_view> host)
{
	if (not host)
		return _configs[0];
	for (auto it : _configs)
	{
		if (it.server_name[0] == host)
			return it;
	}
	return _configs[0];
}

/**
 * @brief Processes and validates the request.
 * Creates a new event handler based on the ResponseType (Fetch, Upload, Delete, CGI)
 */
void	ClientHandler::_process_request()
{
	LOG_NOTICE("Processing request...");

	auto handler 		= HandlerFactory::create_handler(request.get_type());
	Config config 		= _resolve_config(request.get_value("host"));
	response 			= handler->handle_request(request, config);
	ResponseType type 	= response.get_type();

	if (type == ResponseType::Fetch || type == ResponseType::Upload)
	{
		_add_file_handler(type);
	}
	else if (type == ResponseType::Error)
	{
		if (not request.get_file().path.empty())
			_add_file_handler(type);
		else
			_state = State::Ready;
	}
	else
		_state = State::Ready;
}

/**
 * @brief Creates a file_handler, which is responsible for reading/writing from/to files
 */
void	ClientHandler::_add_file_handler(ResponseType type)
{
	short mask = 0;

	if (type == ResponseType::Fetch || type == ResponseType::Error)
	{
		mask = POLLIN;
		_state = State::FetchingFile;
		LOG_NOTICE("Creating Fetch FileHandler with file " << request.get_file().path);
	}
	else if (type == ResponseType::Upload)
	{
		mask = POLLOUT;
		_state = State::UploadingFile;
		LOG_NOTICE("Creating Upload FileHandler with file " << request.get_file().path << "/" << request.get_file().name);
	}
	_file_handler = new FileHandler(request.get_file(), type);
	if (_file_handler->get_fd() < 0)
	{
		LOG_ERROR("Opening file didn't work out");
		exit(123);
	}
	Action<FileHandler> *file_action = new Action<FileHandler>(_file_handler, &FileHandler::handle_file);
	_connection_manager.add(_file_handler->get_fd(), mask, file_action);
}

/**
 * @brief Returns true if elapsed time is bigger than TIME_OUT
 */
bool	ClientHandler::_is_timeout()
{
	return _timer.elapsed_time().count() > TIME_OUT;
}

/**
 * @brief Removes and closes the client and/or filehandler connection
 */
void	ClientHandler::_close_connection()
{
	if (_file_handler != nullptr)
		_connection_manager.remove(_file_handler->get_fd());
	_connection_manager.remove(_socket.get_fd());
}
