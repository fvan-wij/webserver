#include "ConnectionManager.hpp"
#include "Logger.hpp"
#include "ConfigParser.hpp"
#include <Utility.hpp>

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <meta.hpp>
#include <cstring>
#include <sys/poll.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

static void	cleanup_handlers(ConnectionManager& cm, std::vector<pollfd>& pfds)
{
	for (auto fd : pfds)
	{
		cm.remove(fd.fd);
	}
}

static void poll_loop(ConnectionManager &cm)
{
	bool	should_quit = false;

	while (!should_quit)
	{
		std::vector<pollfd> &pfds = cm.get_pfds();
		int n_ready = ::poll(pfds.data(), pfds.size(), POLL_TIMEOUT);
		if (n_ready < 0)
		{
			cleanup_handlers(cm, pfds);
			should_quit = true;
		}
		else if (n_ready > 0)
		{
			cm.handle_pfd_events();
		}
	}
}

int main(int argc, char *argv[], char *envp[])
{
	std::vector<Config>		configs;
	ConnectionManager		cm(envp);

	if (argc == 2 && argv[1] && Utility::check_extension(argv[1], ".conf"))
	{
		configs = parse_config(argv[1]);
	}
	else
	{
		LOG_ERROR("Config is invalid or not present!");
		return 1;
	}

	try 
	{
		cm.add_listeners(configs);
	}
	catch (std::exception &e)
	{
		LOG_ERROR("Error: " << e.what());
		return 1;
	}
	poll_loop(cm);
	return 0;
}
