#include "Server.hpp"
#include "HttpRequest.hpp"
#include "HandlerFactory.hpp"
#include <cstring>
#include "Logger.hpp"


int main()
{
	std::vector<Server> servers;

	servers.push_back({{8080, 8081}}); 
	servers.push_back({{9090, 9091}}); 

	LOG_NOTICE("Starting server(s)");
	for(const Server &s : servers)
		LOG_NOTICE(s);

	while (1)
	{
		for(auto &s : servers)
		{
			if (s.should_exit())
				return 0;
			else if (s.poll() > 0)
				s.handle_events();
		}
	}
	return 0;
}

