#pragma once

#include <vector>
#include <string>

typedef struct s_location {
	std::vector<std::string>	allowed_methods = 
	{
		{"GET"},
		{"POST"},
	};
	std::vector<std::string> URIs = 
	{
		{"/index.html"},
		{"/index"},
		{"/images"},
		{"/posts"},
		{"/"},
	};
} t_location;

typedef struct s_config {
	std::string					server_name = "localhost";
	std::vector<std::string>	methods = {"GET", "POST", "DELETE"};
	t_location					location;
} t_config;

