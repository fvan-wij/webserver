#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <optional>

struct Location {
	std::string								path;
	std::vector<std::string>				allowed_methods;
	std::string 							root;
	std::string 							index;
	std::pair<int, std::string>				redirection;
	bool									autoindex;
};

const Location DEFAULT_LOCATION = {
    "/",                            // path
    {"GET", "POST"},                // allowed_methods
    "/var/www/html",                // root
    "index.html",                   // index
	{},// {301, "http://localhost:9090"}, // redirection
	false                           // autoindex

};

struct Config {
	std::vector<std::string>					server_name;
	std::vector<std::pair<std::string, int>>	listen;
	std::vector<std::string>					methods;
	std::unordered_map<std::string, Location>	location;
	std::string									root;
	int											client_max_body_size = 0;
	std::unordered_map<int, std::string>		error_page;
	std::string									index;

	std::optional<std::string>	get_server_name(int index)
	{
		if (server_name.empty() || server_name[index].empty())
			return std::nullopt;
		return server_name[index];
	}
};

const Config DEFAULT_CONFIG = {
    {"default_server"},                        // server_name
    {{"0.0.0.0", 9090}},                    // listen (IP, port)
    {"GET", "POST"},                      // methods
    { {"/", DEFAULT_LOCATION} },          // location map
    "/var/www/html",                      // root
    1048576,                              // client_max_body_size (1MB)
    {{404, "/404.html"}},                 // error_page (404 -> /404.html)
    "index.html",                         // index page
};

void	print_config(const Config& config);
