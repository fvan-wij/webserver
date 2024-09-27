#pragma once

#include <vector>
#include <string>
#include <unordered_map>

typedef struct s_location {
	std::string					path;
	std::vector<std::string>	allowed_methods;
	std::string 				root;
	std::string 				index;
} t_location;

const t_location DEFAULT_LOCATION = {
    "/",                           // path
    {"GET", "POST"},               // allowed_methods
    "/var/www/html",               // root
    "index.html"                   // index
};

typedef struct s_config {
	std::vector<std::string>					server_name;
	std::vector<std::pair<std::string, int>>	listen; 
	std::vector<std::string>					methods;
	std::unordered_map<std::string, t_location>	location;
	std::string									root;
	int											client_max_body_size;
	std::vector<std::pair<int, std::string>>	error_page; 
	std::vector<std::string>					paths;
	std::string									index;
} t_config;

const t_config DEFAULT_CONFIG = {
    {"default_server"},                   // server_name
    {{"0.0.0.0", 9090}},                  // listen (IP, port)
    {"GET", "POST"},                      // methods
    { {"/", DEFAULT_LOCATION} },          // location map
    "/var/www/html",                      // root
    1048576,                              // client_max_body_size (1MB)
    {{404, "/404.html"}},                 // error_page (404 -> /404.html)
    {"/"},                                // paths
    "index.html",                         // index page
};

void	print_listen(t_config config);
void	print_server_name(t_config config);
void	print_methods(std::vector<std::string> methods);
void	print_location(t_config config);
