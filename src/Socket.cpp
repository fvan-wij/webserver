#include "Socket.hpp"
#include <netinet/in.h>


//Socket can be client or listener
Socket::Socket(t_sock_type connection_type, int data)
{
	if (connection_type == LISTENER)
		_init_listener(data);
	else if (connection_type == CLIENT)
		_init_client(data);
}

// Socket::Socket(t_sock_type connection_type, int fd)  //Client connection
// {
// 	// std::cout << "<Constructing client connection...>" << std::endl;
// 	_type = connection_type;
// 	_fd = fd;
// 	if (fd < 0)
// 		std::cout << "(Error in Socket constructor client): fd is invalid" << std::endl;
// }

int 						Socket::get_fd() const
{
	if (_fd <= 0)
		std::cout << "get_sock_fd() returns negative fd, something's off..." << std::endl;
	return _fd;
};

struct sockaddr_in						Socket::get_address() const 
{
	struct sockaddr_in	empty;

	if (_type == LISTENER)
		return _address;
	std::cout << "!WARNING: get_address() returns empty address due to being client!" << std::endl;
	return empty;
};

Socket::~Socket()
{
	// std::cout << "<deconstructing connection>" << std::endl;
}

void 				Socket::_init_listener(int port)
{
	_fd = socket(AF_INET, SOCK_STREAM, 0); //Creates connection
	if (_fd < 0)
	{
		std::cout << "Error occurred: " << strerror(errno) << std::endl;
	}

	_address.sin_family = AF_INET;
	_address.sin_port = htons(port);
	_address.sin_addr.s_addr = INADDR_ANY;

	const int	optval2 = 1;
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &optval2, sizeof(optval2));
	if (bind(_fd, (struct sockaddr *) &_address, sizeof(_address)) < 0) //Binds connection to address
	{
		std::cout << "Error occurred: " << strerror(errno) << std::endl;
		return;
	}
	if (listen(_fd, 5) < 0)
	{
		std::cout << "Error occurred: " << strerror(errno) << std::endl;
		return;
	}
	_type = LISTENER;
}

void					Socket::_init_client(int fd)
{
	_type = CLIENT;
	_fd = fd;
}

