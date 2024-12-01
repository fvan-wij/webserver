#include <HttpListener.hpp>
#include <ClientHandler.hpp>
#include <Socket.hpp>

HttpListener::HttpListener(uint16_t port, ConnectionManager &cm) : _port(port), _socket(SocketType::LISTENER, port), _connection_manager(cm)
{
	const short mask = POLLIN;
	Action<HttpListener> *listener_action = new Action<HttpListener>(this, &HttpListener::listen_handle);
		
	_connection_manager.add(_socket.get_fd(), mask, listener_action);
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

// gets called from the `ConnectionManager::handle_pfd_events`
void				HttpListener::listen_handle(short events)
{
	(void) events;
	const short mask = POLLIN | POLLOUT;
	Socket socket = _socket.accept();

	ClientHandler *client_handler = new ClientHandler(_connection_manager, socket, _configs);

	auto client_action = new Action<ClientHandler>(client_handler, &ClientHandler::handle_request);

	_connection_manager.add(socket.get_fd(), mask, client_action);
	LOG_INFO("Client (fd " << socket.get_fd() << ") connected to: " << " on port: " << socket.get_port());
}
