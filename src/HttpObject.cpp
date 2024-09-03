#include "HttpObject.hpp"

HttpObject::HttpObject(const std::string &buffer) : _type(RequestType::UNINITIALIZED)
{
	std::istringstream 	stream(buffer);
	std::string 		line;
	size_t 				colon_index;
	bool				body = false;

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
				std::string value = line.substr(colon_index + 1, line.length() - colon_index);
				_header.insert(std::pair<std::string, std::string>(key, value));
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
	for (const auto& elem : obj.get_header())
		out << elem.first <<  ":" << elem.second << std::endl;
	out << "\n" << obj.get_body();
	return out;
}

std::string &HttpObject::get_body()
{
	return _body;
}

std::map<std::string, std::string>	&HttpObject::get_header()
{
	return _header;
}

int32_t	HttpObject::get_content_length()
{
	return std::stoi(_header["Content-Length"]);
}
