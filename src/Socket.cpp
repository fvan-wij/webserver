#include "Socket.hpp"
#include <arpa/inet.h>
#include "Logger.hpp"



//Socket can be client or listener
Socket::Socket(SocketType connection_type, int data) : _fd(data), _type(connection_type)	
{
	if (_type == SocketType::LISTENER)
		_init_listener(data);
	else if (_type == SocketType::CLIENT)
		_init_client(data);
}

Socket::Socket(const Socket &other)
{
	*this = other;
}

Socket &Socket::operator=(const Socket &rhs)
{
	if (this != &rhs)
	{
		this->_fd = rhs._fd;
		this->_address = rhs._address;
		this->_type = rhs._type;
	}
	return *this;
}

Socket::~Socket()
{
	LOG_DEBUG(*this << " destroyed");
	_fd = -1;

}



Socket Socket::accept()
{
	int clientFd = ::accept(_fd, nullptr, nullptr);
	if (clientFd == -1)
	{
		UNIMPLEMENTED("EXCEPTION: Failed accepting client on fd: " << _fd << ", " << strerror(errno));
		close(_fd);
	}
	else
	{
		// TODO Use logger
		LOG_INFO("Accepted new client on listening socket fd: " << _fd << " with clientFd " << clientFd);
	}
	return Socket(SocketType::CLIENT, clientFd);

}

std::string Socket::read()
{
	char buffer[SOCKET_READ_SIZE];

	bzero(&buffer, sizeof(buffer));
	if (recv(_fd, buffer, SOCKET_READ_SIZE, 0) == -1)
	{
		UNIMPLEMENTED("recv failed");
	}
	return buffer;
}

void Socket::write(const std::string s)
{
	UNUSED(s);

	ssize_t data_sent = send(_fd, s.c_str(), s.length(), 0);
	if (data_sent == -1)
	{
		UNIMPLEMENTED("send failed");
	}
	if (data_sent != (ssize_t) s.length())
	{
		WARNING("data send is not equal to data passed in");
	}
}

int 						Socket::get_fd() const
{
	if (_fd <= 0)
		std::cout << "get_sock_fd() returns negative fd, something's off..." << std::endl;
	return _fd;
};

std::string Socket::get_address_str() const
{
	// std::cout << "<deconstructing connection>" << std::endl;
	char str[INET_ADDRSTRLEN];
	if (!inet_ntop(AF_INET, &(_address.sin_addr), str, INET_ADDRSTRLEN))
	{
		LOG_ERROR("Failed inet_ntop");
	}
	return str;
}

int Socket::get_port() const
{
	return ntohs(_address.sin_port);
}



void 				Socket::_init_listener(int port)
{
	_fd = socket(AF_INET, SOCK_STREAM, 0); //Creates connection
	if (_fd < 0)
	{
		// TODO Throw error
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
}

void	Socket::_init_client(int fd)
{
	_fd = fd;

	socklen_t len = sizeof(_address);
	if (getsockname(get_fd(), (struct sockaddr *) &_address, &len) == -1)
	{
		LOG_ERROR("Failed getsockname");
	}
}

std::ostream& operator<< (std::ostream& os, const Socket& s)
{
	os << "[" << s.get_address_str() << ":" << s.get_port() << "-" << s.get_fd() << "]";
	return os;
}
