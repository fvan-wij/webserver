#include "HttpObject.hpp"

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

HttpObject::HttpObject() 
{
	
}

HttpObject::HttpObject(const std::string &buffer) 
{
	std::istringstream 			stream(buffer);
	std::string 				line;
	std::vector<std::string>	tokens;
	bool						body = false;

	_parse_request_line(stream);
	while (std::getline(stream, line))
	{
		if (!body)
		{
			_parse_header_line(line);
			if (line[0] == '\r')
				body = true;
		}
		else if (body)
			_body += line;
	}
}

HttpObject::~HttpObject()
{

}

std::ostream & operator << (std::ostream &out, HttpObject &obj)
{
	if (!obj.get_method().empty())
		out << YELLOW << obj.get_method() << " " << obj.get_location() << " " << obj.get_protocol() << std::endl;
	for (const auto& elem : obj.get_header())
		out << elem.first <<  ":" << elem.second << std::endl;
	out << "\n" << obj.get_body() << END;
	return out;
}

const std::string &HttpObject::get_body() const
{
	return _body;
}

std::unordered_map<std::string, std::string>	&HttpObject::get_header()
{
	return _header;
}

std::string	&HttpObject::get_value(const std::string &key)
{
	return _header[key];
}

std::string &HttpObject::get_method()
{
	if (!_method.empty())
		return _method;
	LOG_ERROR("Method is empty!");
	return _method;
}

std::string &HttpObject::get_protocol()
{
	if (!_protocol.empty())
		return _protocol;
	LOG_ERROR("Protocol is empty!");
	return _protocol;
}

std::string &HttpObject::get_location()
{
	if (!_location.empty())
		return _location;
	LOG_ERROR("Location is empty!");
	return _location;
}

void	HttpObject::_parse_request_line(std::istringstream 	&stream)
{
	std::string 				line;
	std::vector<std::string>	tokens;

	std::getline(stream, line);
	tokens = tokenize_string(line, " ");
	if (tokens.size() != 3)
	{
		LOG_ERROR("HttpObject: Missing method, location or protocol!");
		return;
	}
	if (tokens[0] != "GET" && tokens[0] != "POST" && tokens[0] != "DELETE")
	{
		LOG_ERROR("Method not present");
		return;
	}
	_method = tokens[0];
	if (tokens[1][0] != '/')
	{
		LOG_ERROR("Location not present");
		return;
	}
	_location = tokens[1];
	if (tokens[2] != "HTTP/1.1\r")
	{
		LOG_ERROR("Protocol not present");
		return;
	}
	_protocol = tokens[2];
}

void	HttpObject::_parse_header_line(std::string line)
{
	size_t	colon_index;

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
