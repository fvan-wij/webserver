#pragma once

#include "Socket.hpp"
#include "HttpProtocol.hpp"

/**
 * @brief An object that holds socket and protocol data.
 * Protocol data specifies the protocol that is used for the connection.
 * The socket data specifies the socket that is used for the connection.
 * The config data specifies the configuration of the server this connection is associated with.
 */
class ConnectionInfo
{
	public:
		ConnectionInfo(Socket s, HttpProtocol *p, Config c);
		~ConnectionInfo();

		Config 		get_config();
		Socket 			get_socket();
		HttpProtocol 	*get_protocol();

	private:
		Socket			_socket;
		HttpProtocol	*_protocol;
		Config		_config;
};
