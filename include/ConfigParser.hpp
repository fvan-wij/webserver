#pragma once

#include <string>
#include <vector>
#include <Config.hpp>

std::vector<t_config>	parse_config(std::string_view config_path);

class ConfigParser {
	public:
		std::vector<t_config> read(int fd);


};
