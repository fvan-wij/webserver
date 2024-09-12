#pragma once

#include <vector>
#include <string>
#include <unordered_map>

typedef struct s_location {
	std::string	path;
	std::vector<std::string>	allowed_methods;
	std::string root;
	std::string index;
} t_location;

typedef struct s_config {
	std::vector<std::pair<std::string, int>>	listen;
	std::string									root;
	std::string									index;
	std::vector<std::string>					server_name;
	std::vector<std::string>					methods;
	std::unordered_map<std::string, t_location>	location;
	std::vector<std::string>					paths;
	int											client_max_body_size;
	std::vector<std::pair<int, std::string>>	error_page;
} t_config;

void	print_listen(t_config config);
void	print_server_name(t_config config);
void	print_methods(std::vector<std::string> methods);
void	print_location(t_config config);