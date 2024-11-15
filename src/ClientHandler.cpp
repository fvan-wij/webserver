#include <ClientHandler.hpp>
#include "HandlerFactory.hpp"
#include <iostream>

ClientHandler::ClientHandler(ConnectionManager &cm, Socket socket, std::vector<Config>& configs) : _configs(configs), _socket(socket), _connection_manager(cm), _state(State::ParsingHeaders)
{

}

void ClientHandler::handle_request(short events)
{
	if (events & POLLIN)
	{
		std::optional<std::vector<char>> incoming_data = _socket.read();
		if (not incoming_data)
		{
			LOG_ERROR("Failed to read request from client (fd " << _socket.get_fd() << ")");
			_connection_manager.remove(_socket.get_fd());
			return;
		}
		LOG_INFO("Received request from client (fd " << _socket.get_fd() << ")" << " on server: " << _configs[0].server_name[0] << " on port: " << _socket.get_port());
		parse(incoming_data.value());
	}
	else if (events & POLLOUT)
	{
		switch(_state)
		{
			case State::Ready:
				// Gather data
				response.set_body(_file_handler->get_file().data.data());
				// Send data
				_socket.write(response.to_string());
				LOG_DEBUG("Sending data: " << response.to_string());
				_connection_manager.remove(_socket.get_fd());
				if (_file_handler)
					_connection_manager.remove(_file_handler->get_fd());
				break;
			case State::FetchingFile:
				// Wait on FileHandler
				LOG_DEBUG("Waiting on FileHandler...");
				if (_file_handler->is_finished())
					_state = State::Ready;
				break;
			case State::ProcessingRequest:
				{
					LOG_DEBUG("Processing request...");
					auto handler 	= HandlerFactory::create_handler(request.get_type());
					response 		= handler->handle_request(request, _configs[0]);
					if (response.get_type() == ResponseType::Fetch)
					{
						LOG_DEBUG("Creating FileHandler with file " << request.get_file().path);
						_file_handler = new FileHandler(request.get_file());
						_file_handler->open_file();
						short mask = POLLIN | POLLOUT;
						Action<FileHandler> *file_action = new Action<FileHandler>(_file_handler, &FileHandler::handle_file);
						_connection_manager.add(_file_handler->get_fd(), mask, file_action);
						_state = State::FetchingFile;
					}
				}
				break;
			default:
				break;
		}
	}
}

void	ClientHandler::parse(std::vector<char>& data)
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
