#include <HttpListener.hpp>
#include <HttpClientHandler.hpp>
#include <Socket.hpp>

HttpListener::HttpListener(uint16_t port, ConnectionManager &cm) : _port(port), _socket(SocketType::LISTENER, port), _connection_manager(cm)
{
	short mask = POLLIN;
	Action<HttpListener> *action = new Action<HttpListener>(this, &HttpListener::listen_handle);

	_connection_manager.add(_socket.get_fd(), mask, action);
}

void				HttpListener::add_config(Config config)
{
	_configs.push_back(config);
}

std::vector<Config>	HttpListener::get_configs()
{
	return (_configs);
}

uint16_t			HttpListener::get_port()
{
	return (_port);
}

Socket				HttpListener::get_socket()
{
	return (_socket);
}

void				HttpListener::listen_handle()
{
	short mask = POLLIN | POLLOUT;
	Socket socket = _socket.accept();

	HttpClientHandler *client_handler = new HttpClientHandler(_connection_manager, socket, _configs);
	Action<HttpClientHandler> *action = new Action<HttpClientHandler>(client_handler, &HttpClientHandler::handle_request);
	_connection_manager.add(socket.get_fd(), mask, action);


	// LOG_INFO("Client (fd " << socket.get_fd() << ") connected to: " << config.server_name[0] << " on port: " << socket.get_port());

	// ConnectionInfo *new_ci = new ConnectionInfo(socket, new HttpProtocol(config), config);
	// _pfds.push_back({socket.get_fd(), mask, 0});
	// _fd_types.push_back(FdType::CLIENT);
	// _connection_info[socket.get_fd()] = std::shared_ptr<ConnectionInfo>(new_ci);
}