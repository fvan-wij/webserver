#pragma once

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>
#include <vector>

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
		Socket(const Socket &);
		Socket &operator=(const Socket &);
		~Socket();
		
		// Funcs
		Socket 						accept();
		std::vector<char> 			read();
		void 						write(const std::string s);

		// Getters
		int 						get_fd() const;
 		int							get_port() const;
		std::string					get_address_str() const;
		bool						is_listener() const {return _type == SocketType::LISTENER;};
		bool						is_client() const {return _type == SocketType::CLIENT;};


	private:
		int							_fd;
		struct sockaddr_in			_address;
		SocketType					_type;

		void						_init_listener(int port);
		void						_init_client(int fd);
};

std::ostream& operator<< (std::ostream& stream, const Socket& rhs);
bool operator== (const Socket &s1, const Socket &s2);
