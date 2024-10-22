#include <ConnectionInfo.hpp>

ConnectionInfo::ConnectionInfo(Socket s, HttpProtocol *p, t_config c) : _socket(s), _protocol(p), _config(c)
{

}

ConnectionInfo::~ConnectionInfo()
{
	delete _protocol;
}

t_config ConnectionInfo::get_config()
{
	return (_config);
}

Socket ConnectionInfo::get_socket()
{
	return _socket;
}

HttpProtocol *ConnectionInfo::get_protocol()
{
	return _protocol;
}