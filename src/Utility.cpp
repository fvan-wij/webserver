#include "Utility.hpp"

std::optional<int>	Utility::svtoi(std::string_view str)
{
	int result{};
	auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);

	if (ec == std::errc())
	{
		return result;
	}
	else if (ec == std::errc::invalid_argument)
	{
		LOG_ERROR("svtoi: str is not a number");
	}
	else if (ec == std::errc::result_out_of_range)
	{
		LOG_ERROR("svtoi(): number is larger than an int");
	}
	return std::nullopt;
}


std::optional<int>	Utility::svtoi(std::optional<std::string_view> str)
{
	if (!str)
		return std::nullopt;

	int result{};
	auto [ptr, ec] = std::from_chars(str->data(), str->data() + str->size(), result);

	if (ec == std::errc())
	{
		return result;
	}
	else if (ec == std::errc::invalid_argument)
	{
		LOG_ERROR("svtoi: str is not a number");
	}
	else if (ec == std::errc::result_out_of_range)
	{
		LOG_ERROR("svtoi(): number is larger than an int");
	}
	return std::nullopt;
}

std::vector<std::string>	Utility::tokenize_string(std::string string, std::string delimiter)
{
	std::vector<std::string> 	tokens;
	size_t						pos = 0;

	pos = string.find(delimiter);
	while (pos != std::string::npos)
	{
		std::string token = string.substr(0, pos);
		tokens.push_back(token);
		string.erase(0, pos + delimiter.length());
		pos = string.find(delimiter);
	}
	tokens.push_back(string);
	return tokens;
}
