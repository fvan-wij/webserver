#include <Config.hpp>
#include <ConfigParser.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

static std::vector<std::string> tokenize_string(std::string string, std::string delimiter)
{
        std::vector<std::string>        tokens;
        size_t                                          pos = 0;

        pos = string.find(delimiter);
        while (pos != std::string::npos)
        {
			std::string token = string.substr(0, pos);
			if (!token.empty() && token != "\n") {
                tokens.push_back(token);
			}
			string.erase(0, pos + delimiter.length());
			pos = string.find(delimiter);
        }
		if (!string.empty())
    		tokens.push_back(string);
        return tokens;
}

std::vector<t_config>	parse_config(std::string config_path)
{
	std::vector<t_config>		configs;
	std::ifstream				in(config_path, std::ios_base::in);
	std::vector<std::string>	lines;
	std::string					line;

	while (getline(in, line))
	{
		lines = tokenize_string(line, " ");
		for (unsigned x = 0; x < lines.size(); x++)
		{
			std::cout << lines[x] << ',' << std::endl;
		}
	}
	std::cout << std::endl;
	return (configs);
};