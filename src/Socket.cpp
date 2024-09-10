#include "Logger.hpp"
#include "meta.hpp"
#include "Socket.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <sys/types.h>
#include <iostream>



//Socket can be client or listener
Socket::Socket(SocketType connection_type, int data) : _type(connection_type)	
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
		LOG_WARNING("data send is not equal to data passed in");
	}
}



int 						Socket::get_fd() const
{
	if (_fd <= 0)
		LOG_ERROR("socket : " << _fd << ": get_fd() returns negative fd, something's off...");
	return _fd;
};


int Socket::get_port() const
{
	return ntohs(_address.sin_port);
}

std::string Socket::get_address_str() const
{
	char str[INET_ADDRSTRLEN];
	if (!inet_ntop(AF_INET, &(_address.sin_addr), str, INET_ADDRSTRLEN))
	{
		LOG_ERROR("Failed inet_ntop");
	}
	return str;
}



// // TODO Memcmp?
// bool Socket::operator==	(const Socket &rhs) const
// {
// 	return	this->_fd == rhs._fd && this->_type == rhs._type &&
// 			this->_address.sin_addr.s_addr == rhs._address.sin_addr.s_addr;
// }



void 				Socket::_init_listener(int port)
{
	_fd = socket(AF_INET, SOCK_STREAM, 0); //Creates connection
	if (_fd < 0)
	{
		// TODO Throw error
		LOG_ERROR("Error occurred: " << strerror(errno));
	}

	_address.sin_family = AF_INET;
	_address.sin_port = htons(port);
	_address.sin_addr.s_addr = INADDR_ANY;

	const int	optval2 = 1;
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &optval2, sizeof(optval2));
	if (bind(_fd, (struct sockaddr *) &_address, sizeof(_address)) < 0) //Binds connection to address
	{
		LOG_ERROR("Error occurred: " << strerror(errno));
		return;
	}
	if (listen(_fd, 5) < 0)
	{
		LOG_ERROR("Error occurred: " << strerror(errno));
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

bool operator==(const Socket &s1, const Socket &s2)
{
	return	s1.get_fd() == s2.get_fd() &&
			s1.get_port() == s2.get_port() &&
			s1.get_address_str() == s2.get_address_str();
}
