#pragma once
#include <string_view>
#include <optional>
#include <vector>

class Utility {
	public:
		Utility();
		Utility(Utility &&) = default;
		Utility(const Utility &) = default;
		Utility &operator=(Utility &&) = default;
		Utility &operator=(const Utility &) = default;
		~Utility();

		static std::optional<int>		svtoi(std::string_view str);
		static std::optional<int>		svtoi(std::optional<std::string_view> str);
		static std::vector<std::string>	tokenize_string(std::string string, std::string delimiter);
		static bool 					file_exists(std::string_view file_name);
		static bool 					is_multipart_content(std::string value_to_check);
		static std::string				generate_random_string(size_t len);
};

