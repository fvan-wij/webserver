#include <Config.hpp>
#include <ConfigParser.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>

#define BLUE "\033[38;5;4m"
#define RED "\033[38;5;1m"
#define ORANGE "\033[38;5;130m"
#define RESET "\033[0m"

std::string indent(int i, const std::string& sequence) {
	return std::string(i, ' ') + sequence + " ";
}

std::string key_value(const std::string& key, const std::string& value) {
	if (value.empty())
		return BLUE + key + ": " + ORANGE + "\"\"" + RESET;
	return BLUE + key + ": " + ORANGE + "\"" + value + "\"" + RESET;
}

std::string values(const std::vector<std::string>& values) {
	if (values.empty()) {
		return "\"\"";
	}
	std::string s = "\"";
	for (size_t x = 0; x < values.size(); ++x) {
		s += values[x];
		if (x != values.size() - 1) {
			s += "\", \"";
		}
	}
	s += "\"";
	return s;
}

void print_listen(const Config& config) {
	for (const auto& [ip, port] : config.listen) {
		std::cout << indent(1, "├") << BLUE << "listen: " << ORANGE << ip << ":" << port << RESET << std::endl;
	}
}

void print_name_and_values(const std::string& name, const std::vector<std::string>& vs) {
	std::cout << BLUE << name << ": " << ORANGE << values(vs) << RESET << std::endl;
}

void print_error_page(const Config& config) {
	for (const auto& [status, path] : config.error_page) {
		std::cout << indent(1, "├") << BLUE << "error_page: " << ORANGE << status << " \"" << path << "\"" << RESET << std::endl;
	}
}

void print_location(const Config& config) {
	std::cout << indent(1, "└") << "Locations" << std::endl;
	for (const auto& [path, location] : config.location) {
		std::cout << indent(3, "─") << path << std::endl;
		std::cout << indent(5, "├") << key_value("index", location.index) << std::endl;
		std::cout << indent(5, "├");
		print_name_and_values("methods", location.allowed_methods);
		std::cout << indent(5, "├") << key_value("root", location.root) << std::endl;
		std::cout << indent(5, "├") << key_value("autoindex", location.autoindex ? "on" : "off") << std::endl;
		std::cout << indent(5, "└") << BLUE << "redirection(s): " << ORANGE << location.redirection.first << " -> " << location.redirection.second << RESET << std::endl;
	}
}

void print_config(const Config& config) {
	std::cout << "config" << std::endl;
	print_listen(config);
	std::cout << indent(1, "├") << key_value("root", config.root) << std::endl;
	std::cout << indent(1, "├") << key_value("index", config.index) << std::endl;
	std::cout << indent(1, "├");
	print_name_and_values("server_name(s)", config.server_name);
	std::cout << indent(1, "├");
	print_name_and_values("methods", config.methods);
	print_error_page(config);
	std::cout << indent(1, "├") << key_value("client_max_body_size", std::to_string(config.client_max_body_size)) << std::endl;
	print_location(config);
	std::cout << RESET;
}
