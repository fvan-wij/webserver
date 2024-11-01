#include <Config.hpp>
#include <ConfigParser.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include "Logger.hpp"

static std::vector<std::string> tokenize_string(std::string string, std::string delimiter)
{
        std::vector<std::string>        tokens;
        size_t                          pos = 0;

        pos = string.find(delimiter);
        while (pos != std::string::npos)
        {
			std::string token = string.substr(0, pos);
			if (!token.empty() && token != "\n") 
			{
				token.erase(0, token.find_first_not_of("\t\r\n "));
                tokens.push_back(token);
			}
			string.erase(0, pos + delimiter.length());
			pos = string.find(delimiter);
        }
		if (!string.empty())
    		tokens.push_back(string);
        return tokens;
}

std::string			 parse_root(std::vector<std::string> tokens, unsigned long &i)
{
	std::string root;
	i++;
	root = tokens[i].substr(0, tokens[i].size() - 1);
	i++;
	return (root);
}

std::pair<std::string, int> parse_listen(std::vector<std::string> tokens, unsigned long &i)
{
	std::pair<std::string, int> listen;
	i++;
	listen.first = tokens[i].substr(0, tokens[i].find(":"));
	listen.second = std::stoi(tokens[i].substr(tokens[i].find(":") + 1, tokens[i].size()));
	i++;
	return (listen);
}

std::string parse_index(std::vector<std::string> tokens, unsigned long &i)
{
	std::string index;
	i++;
	index = tokens[i].substr(0, tokens[i].size() - 1);
	i++;
	return (index);
}

std::vector<std::string> parse_server_name(std::vector<std::string> tokens, unsigned long &i)
{
	std::vector<std::string>	server_name;
	i++;
	while (tokens[i].find(";") == std::string::npos)
	{
		server_name.push_back(tokens[i]);
		i++;
	}
	server_name.push_back(tokens[i].substr(0, tokens[i].size() - 1));
	i++;
	return (server_name);
}

std::vector<std::string>	parse_allow_methods(std::vector<std::string> tokens, unsigned long &i)
{
	std::vector<std::string>	methods;
	i++;
	while (tokens[i].find(";") == std::string::npos)
	{
		methods.push_back(tokens[i]);
		i++;
	}
	methods.push_back(tokens[i].substr(0, tokens[i].size() - 1));
	i++;
	return (methods);
}

std::pair<std::string, Location> parse_location(std::vector<std::string> tokens, unsigned long &i)
{
	Location	location;
	std::pair<std::string, Location> entry;
	i++;
	location.path = tokens[i];
	entry.first = location.path;
	while (tokens[i] != "}")
	{
		if (tokens[i] == "root")
		{
			location.root = parse_root(tokens, i);
			continue;
		}
		if (tokens[i] == "index")
		{
			location.index = parse_index(tokens, i);
			continue;
		}
		if (tokens[i] == "allow_methods")
		{
			location.allowed_methods = parse_allow_methods(tokens, i);
			continue;
		}
		i++;
	}
	i++;
	entry.second = location;
	return (entry);
}

int	parse_client_max_body_size(std::vector<std::string> tokens, unsigned long &i)
{
	int client_max_body_size;
	i++;
	client_max_body_size = std::stoi(tokens[i]);
	i++;
	return (client_max_body_size);
}

std::pair<int, std::string> parse_error_page(std::vector<std::string> tokens, unsigned long &i)
{
	std::pair<int, std::string> error_page;
	i++;
	error_page.first = std::stoi(tokens[i]);
	i++;
	error_page.second = tokens[i];
	i++;
	return (error_page);

}

void	print_config(Config config)
{
	std::cout << "config {" << std::endl;
	print_listen(config);
	std::cout << " - root: " << config.root << std::endl;
	std::cout << " - index: " << config.index << std::endl;
	print_server_name(config);
	print_methods(config.methods);
	print_location(config);
	std::cout << " - client_max_body_size: " << config.client_max_body_size << std::endl;
	std::cout << "}" << std::endl;
}

Config	read_config(std::vector<std::string> tokens, unsigned long &i)
{
	Config server_config;
	while (i < tokens.size())
	{
		if (tokens[i] == "root")
		{
			server_config.root = parse_root(tokens, i);
			continue ;
		}
		if (tokens[i] == "listen")
		{
			server_config.listen.push_back(parse_listen(tokens, i));
			continue;
		}
		if (tokens[i] == "index")
		{
			server_config.index = parse_index(tokens, i);
			continue;
		}
		if (tokens[i] == "server_name")
		{
			server_config.server_name = parse_server_name(tokens, i);
			continue;
		}
		if (tokens[i] == "location")
		{
			std::pair<std::string, Location>	location;
			location = parse_location(tokens, i);
			server_config.paths.push_back(location.first);
			server_config.location.insert(location);
			continue;
		}
		if (tokens[i] == "allow_methods")
		{
			server_config.methods = parse_allow_methods(tokens, i);
			continue;
		}
		if (tokens[i] == "error_page")
		{
			server_config.error_page.push_back(parse_error_page(tokens, i));
			continue;
		}
		if (tokens[i] == "client_max_body_size")
		{
			server_config.client_max_body_size = parse_client_max_body_size(tokens, i);
			continue;
		}
		if (tokens[i] == "}")
		{
			break ;
		}
		i++;
	}
	// print_config(server_config);
	return (server_config);
}
std::vector<Config>	parse_config(std::string_view config_path)
{
	std::vector<Config>			configs;
	std::ifstream				in(config_path.data(), std::ios_base::in);
	std::vector<std::string>	tokenized_line;
	std::string					line;
	std::vector<std::string>	tokens;

	while (getline(in, line))
	{
		tokenized_line = tokenize_string(line, " ");
		if (tokenized_line.empty() || tokenized_line[0] == "#")
			continue;
		for (unsigned x = 0; x < tokenized_line.size(); x++)
		{
			tokens.push_back(tokenized_line[x]);
		}
	}
	unsigned long	i;
	i = 0;
	while (i < tokens.size())
	{
		if (tokens[i] == "server\0")
		{
			Config server_config;
			server_config = read_config(tokens, i);
			configs.push_back(server_config);
		}
		i++;
	}
	return (configs);
}
