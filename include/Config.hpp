#pragma once

#include <vector>
#include <string>
#include <unordered_map>

typedef struct s_location {
	std::string					path = {"/"};
	std::vector<std::string>	allowed_methods;
	std::string 				root = {"/index.html"};
	std::string 				index = {"index.html"};
} t_location;

static t_location TEST_LOCATION;

typedef struct s_config {
	std::vector<std::string>					server_name;
	std::vector<std::pair<std::string, int>>	listen; 
	std::vector<std::string>					methods;
	std::unordered_map<std::string, t_location>	location;
	std::string									root;
	int											client_max_body_size;
	std::vector<std::pair<int, std::string>>	error_page = {{402, "/error/402.html"}, {403, "/error/403.html"}};
	std::vector<std::string>					paths;
	std::string									index;
} t_config;

static t_config TEST_CONFIG;

void	print_listen(t_config config);
void	print_server_name(t_config config);
void	print_methods(std::vector<std::string> methods);
void	print_location(t_config config);
