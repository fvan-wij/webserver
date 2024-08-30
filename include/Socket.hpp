#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <iostream>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>

typedef enum t_sock_type {
	LISTENER = 0,
	CLIENT = 1,
} t_sock_type;

class Socket
{
	public:

		//Constructors
		Socket();
		Socket(t_sock_type type, int data); //Listener socket constructor, constructs listener or client based on type argument
		~Socket();
		
		//Member functions
		int 						get_fd() const;
		struct sockaddr_in			get_address() const;
		bool						is_listener() {return _type == LISTENER;};
		bool						is_client() {return _type == CLIENT;};

	private:
		int							_fd;
		struct sockaddr_in			_address;
		t_sock_type					_type;
		// ServerImplementation	

		void						_init_listener(int port);
		void						_init_client(int fd);
};
