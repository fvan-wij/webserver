#include "HttpObject.hpp"

HttpObject::HttpObject(const std::string &buffer) 
{
	std::istringstream 	stream(buffer);
	std::string 		line;
	std::string			method;
	size_t 				colon_index, protocol_index, location_index;
	bool				body = false;

	std::getline(stream, line);
	method = line.substr(0, 3);
	if (method != "GET" && method != "POST" && method != "DELETE")
		LOG_ERROR("Method not supported: " << method);
	_method = method;

	protocol_index = line.find("HTTP/1.1");
	if (protocol_index == std::string::npos)
		LOG_ERROR("Protocol not apparent in header file");
	_protocol = line.substr(protocol_index, line.length());

	location_index = line.find("/");
	if (location_index == std::string::npos)
		LOG_ERROR("location not apparent in header file");
	_location = line.substr(location_index, (protocol_index - location_index) - 1);

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

std::string &HttpObject::get_body()
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
	LOG_ERROR("Protocol is empty!");
	return _location;
}
