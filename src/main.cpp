#include "ConnectionManager.hpp"
#include "ConnectionInfo.hpp"
#include "Logger.hpp"
#include "ConfigParser.hpp"

#include <cstring>
#include <optional>
#include <string>
#include <sys/poll.h>
#include <vector>
#include <iostream>

void poll_loop(ConnectionManager &cm, char *envp[])
{
	while (1)
	{
		std::vector<pollfd> &pfds = cm.get_pfds();
		int n_ready = ::poll(pfds.data(), pfds.size(), POLL_TIMEOUT);
		if (n_ready > 0)
			cm.iterate_fds(envp);
	}
}

static bool	check_extension(const std::string &file, const std::string &ext)
{
  	return ext.length() <= file.length() && std::equal(ext.rbegin(), ext.rend(), file.rbegin());
}

int main(int argc, char *argv[], char *envp[])
{
	std::vector<Config>	configs;
	ConnectionManager		cm;

	if (argc == 2 && argv[1] && check_extension(argv[1], ".conf"))
	{
		configs = parse_config(argv[1]);
	}
	else
	{
		LOG_ERROR("Config is invalid or not present, using DEFAULT_CONFIG");
		configs.push_back(DEFAULT_CONFIG);
	}
	cm.add_listeners(configs);
	poll_loop(cm, envp);
}
