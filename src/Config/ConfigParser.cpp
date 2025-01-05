#include <Config.hpp>
#include <ConfigParser.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <cstring>
#include "Logger.hpp"

static std::vector<std::string> tokenize_string(std::string string, std::string delimiters)
{
		std::vector<std::string>	tokens;
		size_t						start = 0;
		size_t						end = 0;

		start = string.find_first_not_of(delimiters, end);
		while (start != std::string::npos)
		{
			end = string.find_first_of(delimiters, start);
			tokens.push_back(string.substr(start, end - start));
			start = string.find_first_not_of(delimiters, end);
		}
		return tokens;
}

std::string	parse_string(std::vector<std::string> tokens, unsigned long &i)
{
	i++;
	std::string str = tokens[i].substr(0, tokens[i].size() - 1);
	i++;
	return (str);
}

std::vector<std::string> parse_list(std::vector<std::string> tokens, unsigned long &i)
{
	std::vector<std::string>	list;
	i++;
	while (tokens[i].find(";") == std::string::npos)
	{
		list.push_back(tokens[i]);
		i++;
	}
	list.push_back(tokens[i].substr(0, tokens[i].size() - 1));
	i++;
	return list;
}

std::pair<std::string, int> parse_listen(std::vector<std::string> tokens, unsigned long &i)
{
	i++;
	std::string address = tokens[i].substr(0, tokens[i].find(":"));
	int port = std::stoi(tokens[i].substr(tokens[i].find(":") + 1, tokens[i].size()));
	i++;
	return {address, port};
}

std::pair<int, std::string> parse_redirection(std::vector<std::string>& tokens, unsigned long &i)
{
	i++;
	int code = std::stoi(tokens[i]);
	i++;
	std::string url = tokens[i].substr(0, tokens[i].size() - 1);
	i++;
	return {code, url};
}

bool parse_autoindex(std::vector<std::string> tokens, unsigned long &i)
{
	i++;
	bool autoindex = tokens[i] == "on;" ? true : false;
	i++;
	return (autoindex);
}

int	parse_client_max_body_size(std::vector<std::string> tokens, unsigned long &i)
{
	i++;
	int client_max_body_size = std::stoi(tokens[i]);
	i++;
	return (client_max_body_size);
}

std::pair<int, std::string> parse_error_page(std::vector<std::string> tokens, unsigned long &i)
{
	i++;
	int code = std::stoi(tokens[i]);
	i++;
	std::string path = tokens[i].substr(0, tokens[i].size() - 1);
	i++;
	return {code, path};

}

std::pair<std::string, Location> parse_location(std::vector<std::string> tokens, unsigned long &i)
{
	Location	location;
	location.autoindex = false;
	i++;
	location.path = tokens[i];
	while (tokens[i] != "}")
	{
		if (tokens[i] == "root")
			location.root = parse_string(tokens, i);
		else if (tokens[i] == "index")
			location.index = parse_string(tokens, i);
		else if (tokens[i] == "allow_methods")
			location.allowed_methods = parse_list(tokens, i);
		else if (tokens[i] == "return")
			location.redirection = parse_redirection(tokens, i);
		else if (tokens[i] == "autoindex")
			location.autoindex = parse_autoindex(tokens, i);
		else
			i++;
	}
	i++;
	return {location.path, location};
}

Config	read_config(std::vector<std::string> tokens, unsigned long &i)
{
	Config server_config;
	while (i < tokens.size())
	{
		if (tokens[i] == "root")
			server_config.root = parse_string(tokens, i);
		else if (tokens[i] == "listen")
			server_config.listen.push_back(parse_listen(tokens, i));
		else if (tokens[i] == "index")
			server_config.index = parse_string(tokens, i);
		else if (tokens[i] == "server_name")
			server_config.server_name = parse_list(tokens, i);
		else if (tokens[i] == "allow_methods")
			server_config.methods = parse_list(tokens, i);
		else if (tokens[i] == "error_page")
			server_config.error_page.insert(parse_error_page(tokens, i));
		else if (tokens[i] == "client_max_body_size")
			server_config.client_max_body_size = parse_client_max_body_size(tokens, i);
		else if (tokens[i] == "location")
			server_config.location.insert(parse_location(tokens, i));
		else if (tokens[i] == "}")
			break ;
		else
			i++;
	}
	print_config(server_config);
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
		tokenized_line = tokenize_string(line, "\t\r\n ");
		if (tokenized_line.empty() || tokenized_line[0] == "#")
			continue;
		tokens.insert(tokens.end(), tokenized_line.begin(), tokenized_line.end());
	}
	unsigned long i = 0;
	while (i < tokens.size())
	{
		if (tokens[i] == "server")
			configs.push_back(read_config(tokens, i));
		i++;
	}
	return (configs);
}
