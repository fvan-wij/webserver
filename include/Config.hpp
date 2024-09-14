#pragma once

#include <vector>
#include <string>
#include <unordered_map>

typedef struct s_location {
	std::string	path = {"/"};
	std::vector<std::string>	allowed_methods = 
	{
		{"GET"},
		{"POST"},
		{"DELETE"},
	};
	std::string root = {"/index.html"};
	std::string index = {"index.html"};
} t_location;

static t_location TEST_LOCATION;

typedef struct s_config {
	std::string									server_name = "localhost";
	std::vector<std::pair<std::string, int>>	listen = {{"localhost", 9090}, {"localhost", 9091}};
	std::vector<std::string>					methods = {"GET", "POST"};
	std::unordered_map<std::string, t_location>	location_map = {{"/", TEST_LOCATION}};
	std::string									root = "/var/www/html";
	int											client_max_body_size = 4096;
	std::vector<std::pair<int, std::string>>	error_page = {{402, "/error/402.html"}, {403, "/error/403.html"}};
} t_config;

static t_config TEST_CONFIG;
