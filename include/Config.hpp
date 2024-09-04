#pragma once

#include <vector>
#include <string>

typedef struct s_location {

} t_location;

typedef struct s_config {

	std::vector<std::string>		root;
	std::vector<std::string>		index;
	uint32_t						client_max_body_size;
	t_location						location;
} t_config;