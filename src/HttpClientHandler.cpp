#include <HttpClientHandler.hpp>
#include <iostream>

HttpClientHandler::HttpClientHandler(ConnectionManager &cm, Socket socket, std::vector<Config> configs) : _configs(configs), _socket(socket), _connection_manager(cm)
{
}

void HttpClientHandler::handle_request(short events)
{
	if (events & POLLIN)
	{
		// _client_read_data();
		_request = _socket.read();
		if (!_request)
		{
			LOG_ERROR("Failed to read request from client (fd " << _socket.get_fd() << ")");
			_connection_manager.remove(_socket.get_fd());
			return;
		}
		LOG_INFO("Received request from client (fd " << _socket.get_fd() << ")" << " on server: " << _configs[0].server_name[0] << " on port: " << _socket.get_port());
		_protocol.set_config(_configs[0]);

		_protocol.parse_data(_request.value());
		if (_protocol.response.get_type() == ResponseType::CGI && !_protocol.is_cgi_running())
		{
			// protocol->start_cgi(envp);
			// LOG_INFO("Starting CGI on port: " << ci.get_socket().get_port());
			// add_pipe(pfd.fd, protocol->get_pipe_fd());
		}
	}
	else if (events & POLLOUT)
	{
		if (_protocol.response.is_ready())
		{
			std::string data = _protocol.get_data();
			LOG_INFO("Sending response..." << _protocol.response.get_status_code() << " " << _protocol.response.get_status_mssg());
			_socket.write(data);
			if (_protocol.response.get_type() == ResponseType::CGI) // Remove pipe_fd && pipe type
			{
				// remove_pipe(pfd.fd);
			}
			_connection_manager.remove(_socket.get_fd());
		}
		else if (_protocol.response.get_type() == ResponseType::Upload)
		{
			_protocol.poll_upload();
		}
		else if (_protocol.response.get_type() == ResponseType::Fetch) // Note: Fetching requires both reading and writing... but does both when there's a POLLOUT revent.
		{
			// LOG_INFO("fd: " << pfd.fd << " POLLOUT (Fetch file)");
			_protocol.poll_fetch();
		}
	}
	// _response = _request;
	// _socket.write(_response);
}