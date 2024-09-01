#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <iostream>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>

enum class SocketType 
{
	LISTENER,
	CLIENT,
};

class Socket
{
	public:

		//Constructors
		Socket();
		Socket(SocketType type, int data); //Listener socket constructor, constructs listener or client based on type argument
		~Socket();
		
		// Funcs
		Socket 						accept();

		// Getters
		int 						get_fd() const;
		struct sockaddr_in			get_address() const;
		bool						is_listener() {return _type == SocketType::LISTENER;};
		bool						is_client() {return _type == SocketType::CLIENT;};

	private:
		int							_fd;
		struct sockaddr_in			_address;
		SocketType					_type;
		// ServerImplementation	

		void						_init_listener(int port);
		void						_init_client(int fd);
};
