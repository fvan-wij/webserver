#include <Config.hpp>
#include <ConfigParser.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

static std::vector<std::string> tokenize_string(std::string string, std::string delimiter)
{
        std::vector<std::string>        tokens;
        size_t                          pos = 0;

        pos = string.find(delimiter);
        while (pos != std::string::npos)
        {
			std::string token = string.substr(0, pos);
			if (!token.empty() && token != "\n") {
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

std::vector<std::string> parse_listen(std::vector<std::string> tokens, unsigned long &i)
{
	std::vector<std::string> listen;
	i++;
	while (tokens[i].find(';') == std::string::npos)
	{
	std::cout << "lsiten " << tokens[i] << std::endl;
		listen.push_back(tokens[i]);
		i++;
	}
	listen.push_back(tokens[i].substr(0, tokens[i].length() - 1));
	return (listen);
}

std::vector<std::string> parse_index(std::vector<std::string> tokens, unsigned long &i)
{

}

std::string parse_server_name(std::vector<std::string> tokens, unsigned long &i)
{
	std::string server_name;

	return (server_name);
}

t_location parse_location(std::vector<std::string> tokens, unsigned long &i)
{
	t_location	location;


	return (location);
}


t_config	read_config(std::vector<std::string> tokens, unsigned long &i)
{
	t_config server_config;
	while (i < tokens.size())
	{
		if (tokens[i] == "root")
			server_config.root = parse_root(tokens, i);
		if (tokens[i] == "listen")
			server_config.listen = parse_listen(tokens, i);
		// if (tokens[i] == "index")
		// 	server_config.index = parse_index(tokens, i);
		// if (tokens[i] == "server_name")
		// 	server_config.server_name = parse_server_name(tokens, i);
		// if (tokens[i] == "location")
		// 	server_config.location = parse_location(tokens, i);
		if (tokens[i] == "}")
		{
			std::cout << "end of server object" << std::endl;
			std::cout << server_config.root << std::endl;
			std::cout << server_config.listen[0] << " " << server_config.listen[1] << std::endl;
			return (server_config);
		}
		std::cout << "i: " << i << std::endl;
		i++;
	}
	return (server_config);
}

std::vector<t_config>	parse_config(std::string config_path)
{
	std::vector<t_config>		configs;
	std::ifstream				in(config_path, std::ios_base::in);
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
	for (unsigned i = 0; i < tokens.size(); i++)
	{
		std::cout << tokens[i] << ' ';
	}
	unsigned long	i;
	i = 0;
	while (i < tokens.size())
	{
		if (tokens[i] == "server\0")
		{
			t_config server_config;
			std::cout << "server start" << std::endl;
			server_config = read_config(tokens, i);
		}
		i++;
	}
	std::cout << std::endl;
	std::cout << std::endl;
	return (configs);
};