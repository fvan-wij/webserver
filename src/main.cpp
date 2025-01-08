#include "ConnectionManager.hpp"
#include "Logger.hpp"
#include "ConfigParser.hpp"
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <meta.hpp>

#include <cstring>
#include <string>
#include <sys/poll.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

static void poll_loop(ConnectionManager &cm, char *envp[])
{
	while (1)
	{
		std::vector<pollfd> &pfds = cm.get_pfds();
		int n_ready = ::poll(pfds.data(), pfds.size(), POLL_TIMEOUT);
		if (n_ready > 0)
			cm.handle_pfd_events(envp);
	}
}

static bool	check_extension(const std::string &file, const std::string &ext)
{
  	return ext.length() <= file.length() && std::equal(ext.rbegin(), ext.rend(), file.rbegin());
}


static void write_pid_to_file(std::string file)
{
	std::ofstream out(file);
	out << getpid() << std::endl;
	out.close();
}

int main(int argc, char *argv[], char *envp[])
{
	std::vector<Config>	configs;
	ConnectionManager		cm(envp);


	write_pid_to_file("pid.txt");


	if (argc == 2 && argv[1] && check_extension(argv[1], ".conf"))
	{
		configs = parse_config(argv[1]);
	}
	else
	{
		LOG_ERROR("Config is invalid or not present, using DEFAULT_CONFIG");
		configs.push_back(DEFAULT_CONFIG);
	}
	bool should_exit = false;
	try 
	{
		cm.add_listeners(configs);
	}
	catch (std::exception &e)
	{
		should_exit = true;
	}
	if (!should_exit)
		poll_loop(cm, envp);
	return should_exit;
}
