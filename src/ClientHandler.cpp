#include "HttpResponse.hpp"
#include "Logger.hpp"
#include <ClientHandler.hpp>
#include <HandlerFactory.hpp>
#include <iostream>
#include <Utility.hpp>
#include <string>

/**
 * @brief ClientHandler; responsible for reading and sending data from and to the client.
 * Acts as as finite-state machine based on its state (uploading, fetching, processingCGI).
 * Triggers timeouts when the timeout time is exceeded
 *
 * @param cm: ConnectionManager object
 * @param socket: Socket object
 * @param configs: vector of configs
 */

ClientHandler::ClientHandler(ConnectionManager& cm, Socket socket, std::vector<Config>& configs, char *envp[])
	: _configs(configs), _socket(socket), _connection_manager(cm), _file_handler(nullptr), _state(State::ParsingHeaders), _timed_out(false), _envp(envp)
{

}

/**
 * @brief handles incoming data (POLLIN) and/or outgoing data (POLLOUT) based on the set 'events'
 *
 * @param events: POLLIN and/or POLLOUT
 */
void ClientHandler::handle_request(short revents)
{

	try
	{
		if (not _timed_out)
		{
			_poll_timeout_timer();
			_poll_file_handler();
		}
		if (revents & POLLIN)
		{
			_handle_incoming_data();
		}
		if (revents & POLLOUT)
		{
			_handle_outgoing_data();
		}
	}
	catch (const HttpException& e)
	{
		LOG_ERROR(std::to_string(e.status()) << " " << e.what());
		_build_error_response(e.status(), e.what(), _retrieve_error_path(e.status(), _configs[0]));
	}
	catch (const HttpRedirection& e)
	{
		LOG_ERROR(std::to_string(e.status()) << " " << HTTP_REDIRECTION.at(e.status()) << " redirecting to " << e.what());
		_build_redirection_response(e.status(), e.what());
	}
	catch (const ClosedConnectionException& e)
	{
		LOG_ERROR(e.what());
		_close_connection();
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
		LOG_ERROR("Failed to read _request from client (fd " << _socket.get_fd() << ")");
		throw (HttpException(400, "Bad Request"));
	}

	LOG_INFO("Received _request from client (fd " << _socket.get_fd() << ")" << " on port: " << _socket.get_port());


	_parse(incoming_data.value());
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
		case State::ProcessingFileIO:
			_poll_file_handler();
			break;
		case State::ProcessingRequest:
			_process_request();
			break;
		case State::ProcessingCGI:
			_poll_cgi();
			break;
		default:
			break;
	}
}


void ClientHandler::_poll_cgi()
{

	if (_cgi.poll())
	{
		_state = State::Ready;
		LOG_NOTICE("CGI OUTPUT: " << _cgi.get_buffer());
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
	{
		std::string path("." + config.root + error);
		if (Utility::file_exists(path))
		{
			return ("." + config.root + error);
		}
	}
	return std::nullopt;
}

void	ClientHandler::_build_error_response(int status_code, const std::string& message, std::optional<std::string> error_path)
{

	_response.set_type(ResponseType::Error);

	if (error_path)
	{

		_request.set_file_path(error_path.value());
		_response.set_status_code(status_code);
		_response.set_status_mssg(message);
    
		_add_file_handler(ResponseType::Error);
		_state = State::ProcessingFileIO;
		return;
	}
	else
	{
		_response.set_status_code(status_code);
		_response.set_status_mssg(message);
		_response.set_body("<h1>" + std::to_string(_response.get_status_code()) + " " + _response.get_status_mssg() + "</h1>\r\n");
		_state = State::Ready;
	}
}

void	ClientHandler::_build_redirection_response(int status_code, const std::string& message)
{
	_response.set_status_code(status_code);
	_response.set_status_mssg(HTTP_REDIRECTION.at(status_code));
	_response.insert_header({"Location", message});

	_state = State::Ready;
}

/**
 * @brief Processes and validates the _request.
 * Creates a new event handler based on the ResponseType (Fetch, Upload, Delete, CGI)
 */
void	ClientHandler::_process_request()
{
	auto handler 		= HandlerFactory::create_handler(_request.get_type());
  	_config 			= _resolve_config(_request.get_value("Host"));
	_response 			= handler->build_response(_request, _config, _port);

	ResponseType type 	= _response.get_type();



	if (type == ResponseType::Fetch || type == ResponseType::Upload)
	{
		try
		{
			_add_file_handler(type);
		}
		catch (HttpException& e)
		{
			_build_error_response(e.status(), e.what(), _retrieve_error_path(400, _configs[0]));
		}
	}
	// NOTE: leftoff 
	else if (type == ResponseType::CGI)
	{
		std::string body_buf(_request.get_body_buffer().begin(), _request.get_body_buffer().end());
		LOG_DEBUG("body_buffer: " + body_buf);



		_cgi.verify(_response.get_path(), _request.get_url_parameters_as_string(), body_buf, _envp);
		_cgi.start(_envp);
		_state = State::ProcessingCGI;
	}
	else
		_state = State::Ready;
}

/**
 * @brief Parses the chunk of incoming data.
 * With each parsing call, the data is being moved to the _request object
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
				case State::ParsingChunkedBody:
					_state = _request.parse_body_chunked(data);
					break;
				default:
					break;
		}
	}
}

/**
 * @brief Sends a _response based on the given type
 *
 * @param type: ResponseType::(Fetch, Upload, Delete, CGI)
 */
void	ClientHandler::_send_response(ResponseType type)
{
	if (type == ResponseType::Fetch || type == ResponseType::Error)
	{
		std::vector<char>& data = _file.data;
		if (not data.empty())
		{
			_response.set_body(std::string(data.begin(), data.end()));
		}
	}
	else if (type == ResponseType::CGI)
	{
		_response.set_body(_cgi.get_buffer());
	}

	_response.insert_header({"Server", "webserv"});
	_response.insert_header({"Virtual-Host", _config.get_server_name(0).value_or("")});
	_response.insert_header({"Connection", "close"});
	_response.insert_header({"Content-Length", std::to_string(_response.get_body().size())});
	_socket.write(_response.to_string());
	LOG_NOTICE("(Server) " << _config.get_server_name(0).value_or("") << ": Response sent (fd " << _socket.get_fd() << "): ");
	const auto& headers = _response.get_header();
	std::cout << std::to_string(_response.get_status_code()) << " " << _response.get_status_mssg() << "\n";
	for (const auto& [key, val] : headers)
	{
		std::cout << key << " : " << val << "\n";
	}
	std::cout << "Body\n";
	std::cout << "Size: " << _response.get_body().size();
	std::cout << "File: " << _request.get_file().name;
	std::cout << std::endl;

	_close_connection();
}

Config	ClientHandler::_resolve_config(std::optional<std::string_view> host)
{
	if (not host)
		return _configs[0];
	for (auto conf : _configs)
	{
		if (conf.get_server_name(0).value_or("") == host)
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
	// NOTE if we're doing a `ResponseType::Upload` check if `_request.get_file()` is empty
	_file_handler = new FileHandler(_request.get_file(), type);
	Action<FileHandler> *file_action = new Action<FileHandler>(_file_handler, &FileHandler::handle_file);
	LOG_NOTICE("FileHandler (fd " << _request.get_file().fd << ")");
	_connection_manager.add(_file_handler->get_fd(), mask, file_action);
}

/**
 * @brief Polls the _file_handler and sets the finite-state machine to ready when the _file_handler is done
 */
void ClientHandler::_poll_file_handler()
{
	if (not _file_handler)
	{
		return;
	}
	else if (_file_handler->error())
	{
		_connection_manager.remove(_file_handler->get_fd());
		_file_handler = nullptr;
		_build_error_response(400, "Bad Request", _retrieve_error_path(400, _configs[0]));
	}
	else if (_file_handler->is_finished())
	{
		LOG_NOTICE("(fd " << _socket.get_fd() << ") FileHandler is finished (fd " << _file_handler->get_fd() << ")");
		LOG_INFO("File handler (fd " << _file_handler->get_fd() << ") disconnected");
		_file = _file_handler->get_file();
		_connection_manager.remove(_file_handler->get_fd());
		_file_handler = nullptr;
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
	_cgi.kill();
	LOG_INFO("Client (fd " << _socket.get_fd() << ") disconnected");
	_connection_manager.remove(_socket.get_fd());
}
