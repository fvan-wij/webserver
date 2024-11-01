#include <Config.hpp>
#include <ConfigParser.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>

void	print_listen(Config config)
{
	std::vector<std::pair<std::string, int>> listen;
	listen = config.listen;
	for (unsigned int x = 0; x < listen.size(); x++)
	{
		std::cout << " - listen: " << listen[x].first << " " << listen[x].second << std::endl;
	}
}

void	print_server_name(Config config)
{
	std::cout << " - server_name(s):";
	for (unsigned int x = 0; x < config.server_name.size(); x++)
	{
		std::cout << ", " << config.server_name[x];
	}
	std::cout << std::endl;
}

void	print_methods(std::vector<std::string> methods)
{
	std::cout << " - methods:";
	for (unsigned int x = 0; x < methods.size(); x++)
	{
		std::cout << ", " << methods[x];
	}
	std::cout << std::endl;
}

void	print_location(Config config)
{
	Location location;
	std::cout << " - Locations {" << std::endl;
	for (auto& [path, location]: config.location)
	{
		std::cout << "   - " << path << " {" << std::endl;
		std::cout << "     - index: " << location.index << std::endl;
		std::cout << "    ";
		print_methods(location.allowed_methods);
		std::cout << "     - root: " << location.root << std::endl;
		std::cout << "   - }" << std::endl;
	}
	std::cout << " - }" << std::endl;
}
