#include "Server.hpp"
#include <cstring>
#include <fstream>
#include <ostream>
#include <string>
#include <unistd.h>
#include "Logger.hpp"

void write_pid_to_file(const std::string name)
{
	std::ofstream file(name);

	file << getpid() << std::endl;
	file.close();
}


int main()
{
	std::vector<Server> servers;

	write_pid_to_file("pid.txt");

	// servers.push_back({{8080, 8081}}); 
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

