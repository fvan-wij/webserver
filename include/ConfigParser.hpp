#pragma once

#include <string>
#include <vector>
#include <Config.hpp>

std::vector<Config>	parse_config(std::string_view config_path);

class ConfigParser {
	public:
		std::vector<Config> read(int fd);
};
