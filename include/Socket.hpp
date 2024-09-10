#pragma once

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>
#include <cerrno>
#include <cstring>

#include "meta.hpp"

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
		std::string 				read();
		void 						write(const std::string s);

		// Getters
		int 						get_fd() const;
		struct sockaddr_in			get_address() const;
		bool						is_listener() const {return _type == SocketType::LISTENER;};
		bool						is_client() const {return _type == SocketType::CLIENT;};

	private:
		int							_fd;
		struct sockaddr_in			_address;
		SocketType					_type;

		void						_init_listener(int port);
		void						_init_client(int fd);
};
