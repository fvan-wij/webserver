#include "HttpResponse.hpp"
#include "Logger.hpp"
#include <ClientHandler.hpp>
#include <HandlerFactory.hpp>
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

ClientHandler::ClientHandler(ConnectionManager &cm, Socket socket, std::vector<Config>& configs, char *envp[]) 
	: _configs(configs), _socket(socket), _connection_manager(cm), _file_handler(nullptr), _state(State::ParsingHeaders), _timed_out(false)

{
	_envp = envp;
}

/**
 * @brief handles incoming data (POLLIN) and/or outgoing data (POLLOUT) based on the set 'events'
 *
 * @param events: POLLIN and/or POLLOUT
 */
void ClientHandler::handle_request(short events)
{
	try 
	{
		if (not _timed_out)
		{
			_poll_timeout_timer();
			if (events & POLLIN)
				_handle_incoming_data();
		}
		if (events & POLLOUT)
			_handle_outgoing_data();
	}
	catch (const HttpException& e)
	{
		LOG_ERROR(std::to_string(e.status()) << " " << e.what());
		_build_error_response(e.status(), e.what());
	}
}

/**
 * @brief Reads and parses the incoming data;
 */
void	ClientHandler::_handle_incoming_data()
{
	std::optional<std::vector<char>> incoming_data = _socket.read();

	if (not incoming_data)
	{
		LOG_ERROR("Failed to read request from client (fd " << _socket.get_fd() << ")");
		throw (HttpException(400, "Bad Request"));
	}
	else
	{
		LOG_INFO("Received request from client (fd " << _socket.get_fd() << ")" << " on port: " << _socket.get_port());
		_parse(incoming_data.value());
	}
}

void ClientHandler::_poll_cgi()
{

	if (_cgi.is_running() && _cgi.poll())
	{
		LOG_NOTICE("CGI is finished");
		_state = State::Ready;
		LOG_NOTICE("CGI OUTPUT: " << _cgi.get_buffer());
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
			_send_response(_response.get_type());
			break;
		// TODO This should also poll the CGI
		case State::ProcessingFileIO:
			_poll_file_handler();
			break;
		case State::ProcessingCGI:
			_poll_cgi();
			break;
		case State::ProcessingRequest:
			_process_request();
			break;
		default:
			break;
	}
}

/**
 * @brief Searches the list of error pages and returns the correct one.
 *
 * @param error_code
 * @param config
 * @return std::optional<std::string>
 */
std::optional<std::string> ClientHandler::_retrieve_error_path(int error_code, Config &config)
{
	std::string error = config.error_page[error_code];
	if (!error.empty())
		return ("." + config.root + error);
	return std::nullopt;
}

void	ClientHandler::_build_error_response(int status_code, const std::string& message)
{
	std::optional<std::string> error_path = _retrieve_error_path(status_code, _configs[0]);
	if (error_path)
	{
		_request.set_file_path(error_path.value());
		_add_file_handler(ResponseType::Error);
		_state = State::ProcessingFileIO;
	}
	else
	{
		_response.set_status_code(status_code);
		_response.set_status_mssg(message);
		_response.set_body("\r\n<h1>" + std::to_string(status_code) + " " + message + "</h1>\r\n");
		_state = State::Ready;
	}
	_response.set_type(ResponseType::Error);
}

/**
 * @brief Processes and validates the request. 
 * Creates a new event handler based on the ResponseType (Fetch, Upload, Delete, CGI)
 */
void	ClientHandler::_process_request()
{
	LOG_NOTICE("Processing request...");
	auto handler 		= HandlerFactory::create_handler(_request.get_type());
	_config 			= _resolve_config(_request.get_value("Host"));
	_response 			= handler->build_response(_request, _config);

	ResponseType type 	= _response.get_type();

	std::vector<const char *> args = { "/usr/bin/ls" };
	_cgi.start(args, _envp);
	_state = State::ProcessingCGI;

return ;

	// TODO Try to use a filehandler for reading a CGIs output.
	if (type == ResponseType::Fetch || type == ResponseType::Upload)
	{
		_add_file_handler(type);
	}
	else if (type == ResponseType::CGI)
	{
		std::vector<const char *> args = { "/usr/bin/ls" };
		_cgi.start(args, _envp);
		_state = State::ProcessingCGI;
	}
	else
		_state = State::Ready;
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
				_state = _request.parse_header(data);
				break;
			case State::ParsingBody:
				_state = _request.parse_body(data);
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
	if (type == ResponseType::Fetch || (type == ResponseType::Error && _file_handler))
	{
		std::vector<char>& data = _file_handler->get_file().data;
		if (not data.empty())
		{
			data.push_back('\0');
			_response.set_body(data.data());
			_response.set_server("webserv");
			_response.set_virtual_host(_config.server_name[0]);
			_socket.write(_response.to_string());
			LOG_NOTICE("Response sent to fd " << _socket.get_fd() << ":\n" << _response.to_string());
			_close_connection();
		}
	}
	else
	{
		_socket.write(_response.to_string());
		LOG_NOTICE("Response sent to fd " << _socket.get_fd() << ":\n" << _response.to_string());
		_close_connection();
	}
}

Config	ClientHandler::_resolve_config(std::optional<std::string_view> host)
{
	LOG_DEBUG(host.value_or("No host"));
	if (not host)
		return _configs[0];
	for (auto conf : _configs)
	{
		if (conf.server_name[0] == host)
			return conf;
	}
	return _configs[0];
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
		LOG_NOTICE("Creating Fetch FileHandler with file " << _request.get_file().path);
	}
	else if (type == ResponseType::Upload)
	{
		mask = POLLOUT;
		LOG_NOTICE("Creating Upload FileHandler with file " << _request.get_file().path << "/" << _request.get_file().name);
	}
	_state = State::ProcessingFileIO;
	_file_handler = new FileHandler(_request.get_file(), type);
	Action<FileHandler> *file_action = new Action<FileHandler>(_file_handler, &FileHandler::handle_file);
	_connection_manager.add(_file_handler->get_fd(), mask, file_action);
}

/**
 * @brief Polls the _file_handler and sets the finite-state machine to ready when the _file_handler is done
 */
void ClientHandler::_poll_file_handler()
{
	LOG_NOTICE("Waiting on FileHandler...");

	// TODO Refactor this so it also polls CGI.
	 if (_file_handler->is_finished())
	{
		LOG_NOTICE("FileHandler is finished");
		_state = State::Ready;
	}
}


/**
 * @brief Returns true if elapsed time is bigger than TIME_OUT
 */
void	ClientHandler::_poll_timeout_timer()
{
	if (_timer.elapsed_time().count() > TIME_OUT)
	{
		LOG_ERROR("Client on fd " << _socket.get_fd() << " timed out");
		_timed_out = true;
		throw HttpException(408, "Request Timeout");
	}
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
