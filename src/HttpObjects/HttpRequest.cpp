#include "HttpRequest.hpp"
#include "Logger.hpp"

std::string	HttpRequest::get_method() const
{
	return _method;
}

std::string	HttpRequest::get_uri() const
{
	return _uri;
}

std::string	HttpRequest::get_protocol() const
{
	return _protocol;
}

std::optional<std::string_view>	HttpRequest::get_value(const std::string &key) const
{
	if (_header.find(key) != _header.end())
	{
		return _header.at(key);
	}
	return std::nullopt;
}

void	HttpRequest::set_type(RequestType type)
{
	_type = type;
}

void	HttpRequest::parse(const std::string &buffer)
{
	std::istringstream 			stream(buffer);
	std::string					line;

	_parse_request_line(stream);
	size_t			colon_index;
	bool 			body = false;
	while (std::getline(stream, line))
	{
		if (!body)
		{
			if (line[0] == ':')
				colon_index = line.rfind(':');
			else
				colon_index = line.find(':');
			if (colon_index != std::string::npos)
			{
				std::string key = line.substr(0, colon_index);
				std::string value = line.substr(colon_index + 2, line.length() - colon_index);
				_header.emplace(key, value);
			}
			if (line[0] == '\r')
				body = true;
		}
		// else if (body)
		// 	_body += line;
	}
}

void	HttpRequest::parse_header(const std::string &data)
{
	std::istringstream 			stream(data);
	std::string					line;

	_parse_request_line(stream);
	size_t			colon_index;
	while (std::getline(stream, line))
	{
			if (line[0] == ':')
				colon_index = line.rfind(':');
			else
				colon_index = line.find(':');
			if (colon_index != std::string::npos)
			{
				std::string key = line.substr(0, colon_index);
				std::string value = line.substr(colon_index + 2, line.length() - colon_index);
				_header.emplace(key, value);
			}
	}
	_b_header_parsed = true;
}

void HttpRequest::parse_body(std::vector<char> data)
{
	_body.insert(_body.end(), data.begin(), data.end());
	_b_body_parsed = true;
	LOG_NOTICE("BODY IS PARSED!");
}

static std::vector<std::string>	tokenize_string(std::string string, std::string delimiter)
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

void	HttpRequest::_parse_request_line(std::istringstream 	&stream)
{
	std::string 				line;
	std::vector<std::string>	tokens;

	std::getline(stream, line);
	tokens = tokenize_string(line, " ");
	if (tokens.size() != 3)
		throw HttpException("HttpRequest: Missing method, location or protocol!");
	if (tokens[0] != "GET" && tokens[0] != "POST" && tokens[0] != "DELETE")
		throw HttpException("HttpRequest: Missing or incorrect request method!");
	_method = tokens[0];
	_type = REQUEST_TYPE.at(_method);

	if (tokens[1][0] != '/')
		throw HttpException("HttpRequest: URI not present!");
	_uri = tokens[1];
	if (tokens[2] != "HTTP/1.1\r")
		throw HttpException("HttpRequest: Protocol not present!");
	_protocol = tokens[2];
}

std::ostream & operator << (std::ostream &out, HttpRequest &request)
{
	if (!request.get_method().empty())
		out << YELLOW << request.get_method() << " " << request.get_uri() << " " << request.get_protocol() << std::endl;
	for (const auto& [key, value] : request.get_headers())
		out << key <<  ":" << value << "\n";
	out << END << "\n" << RED << request.get_body().data() << END << std::endl;
	return out;
}

std::ostream & operator << (std::ostream &out, const HttpRequest &request)
{
	if (!request.get_method().empty())
		out << YELLOW << request.get_method() << " " << request.get_uri() << " " << request.get_protocol() << std::endl;
	for (const auto& [key, value] : request.get_headers())
		out << key <<  ":" << value << "\n";
	out << END << "\n" << RED << request.get_body().data() << END << std::endl;
	return out;
}
