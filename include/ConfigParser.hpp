#pragma once

#include <string>
#include <vector>
#include <Config.hpp>
#include <optional>

std::optional<std::vector<t_config>>	parse_config(int argc, std::string config_path);

class ConfigParser {
	public:
		std::vector<t_config> read(int fd);


};
