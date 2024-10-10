// this class will be accessable by both the `VirtualServer` and the main poll loop.


// #include "Server.hpp"
#include "Socket.hpp"
#include "ConnexxionManager.hpp"

class Server;

// adds a client to `_pfd`s list
void ConnectionManager::add(const Server &server, Socket socket)
{

}

void ConnectionManager::remove(const Server &server, Socket &socket)
{

}
