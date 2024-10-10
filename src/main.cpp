#include "Logger.hpp"
#include "Server.hpp"
#include "HttpRequest.hpp"
#include "HandlerFactory.hpp"
#include "ConfigParser.hpp"

#include <cstring>
#include <optional>
#include <sys/poll.h>
#include <vector>

// #define USE_TEST_MAIN

#ifndef USE_TEST_MAIN

std::optional<std::vector<Server>> create_servers(std::vector<t_config> &configs) {
    if (!configs.empty()) {
        LOG_NOTICE("Creating server(s):");
        std::vector<Server> servers;
        for (auto &config : configs) {
            servers.emplace_back(config);
        }
        return servers;
    } else {
        LOG_ERROR("Could not create server(s) from the given config file. Did you supply a config file?");
        return std::nullopt;
    }
}

int	run_servers(std::vector<Server> &servers)
{
	bool should_exit = false;

	while (!should_exit)
	{
		for(Server &s : servers)
		{
			if (s.should_exit())
			{
				should_exit = true;
				return 0;
			}
			else if (s.poll() > 0)
			{
				s.handle_events();
			}
		}
	}
	return 1;
}

int main(int argc, char *argv[])
{
	std::vector<t_config>	configs;

	if (argc == 2 && argv[1])
	{
		configs = parse_config(argv[1]);
	}
	else
	{
		LOG_ERROR("Config is invalid or not present, using DEFAULT_CONFIG");
		configs.push_back(DEFAULT_CONFIG);
		// return -1;
	}



	std::vector<pollfd> &_pfds = ConnectionManager.getPfd();
	while (1)
	{
		int n_ready = ::poll(_pfds.data(), _pfds.size(), POLL_TIMEOUT);
		if (n_ready)
		{
			for (pollfd pfd : _pfds)
			{
				// map current fd  to `VirtualServer`
				// call VirtualServer.handle(pfd)
			}
		}
	}

	if (auto initialized_servers = create_servers(configs))
	{
		return (run_servers(*initialized_servers));
	}
	else 
	{
		LOG_ERROR("Error initializing server(s)");
		return -1;
	}
}

#endif
