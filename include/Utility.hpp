#pragma once
#include <string_view>
#include <optional>
#include <vector>
#include <charconv>
#include "Logger.hpp"

class Utility {
	public:
		Utility();
		Utility(Utility &&) = default;
		Utility(const Utility &) = default;
		Utility &operator=(Utility &&) = default;
		Utility &operator=(const Utility &) = default;
		~Utility();

		static std::optional<int>	svtoi(std::string_view str);
		static std::optional<int>	svtoi(std::optional<std::string_view> str);
		static std::vector<std::string>	tokenize_string(std::string string, std::string delimiter);
};

