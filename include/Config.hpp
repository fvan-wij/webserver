#pragma once

#include <vector>
#include <string>

typedef struct s_location {

} t_location;

typedef struct s_config {

	std::string						root;
	std::vector<std::string>		index;
	std::vector<std::string>		listen;
	uint32_t						client_max_body_size;
	t_location						location;
	std::string						server_name;

} t_config;