#include "HttpRequest.hpp"
#include "Logger.hpp"
#include "Utility.hpp"

#include <algorithm>

HttpRequest::HttpRequest()
{
	_b_header_parsed = false;
	_b_body_parsed = false;
	_b_file = false;
	_b_file_extracted = false;
	_b_file_path_extracted = false;
	_b_boundary_extracted = false;
	_b_file_data_extracted = false;
}

HttpRequest::~HttpRequest()
{

}

std::string_view HttpRequest::get_method() const
{
	return _method;
}

std::string_view HttpRequest::get_uri() const
{
	return _uri;
}

std::filesystem::path HttpRequest::get_uri_as_path() const
{
	return std::filesystem::path(_uri);
}

std::string_view HttpRequest::get_protocol() const
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

void HttpRequest::set_file_upload_path(std::string_view root)
{
	_file.filename.insert(0, "/");
	_file.path = root.data() + _file.filename;
}

State	HttpRequest::parse_header(std::vector<char>& buffer)
{
	std::string_view data_sv(buffer.data(), buffer.size());
	static int x;

	x++;
	LOG_DEBUG("Incoming datachunk on iteration " << x << " = " << buffer.size());
	if (not _b_header_parsed)
	{
		size_t	header_end = data_sv.find("\r\n\r\n", 0); //0 can be removed, right. Right???
		if (header_end != std::string::npos)
		{
			_header_buffer += data_sv.substr(0, header_end);
			_extract_header_fields(data_sv);
			_b_header_parsed = true;
			LOG_DEBUG("Iteration #" << x << "_b_header_parsed = true");
			if (buffer.size() < (SOCKET_READ_SIZE - 1)) // THIS IS A CRUCIAL STEP IN THE SOLUTION, WHLY DOES IT READ BELOW 1024 ON THE FIRST READ CALL WHEN UPLOADING BIG ASS FILE??? WTF?
			{
				buffer.clear();
				return State::BuildingResponse;
			}
			else
				buffer.erase(buffer.begin(), buffer.begin() + header_end + 4);
		}
		else
		{
			_header_buffer.append(buffer.data(), buffer.size());
			buffer.clear();
			LOG_DEBUG("Iteration #" << x << "returning State::ReadingHeaders");
			return State::ParsingHeaders;
		}
	}
	if (not buffer.empty() && _b_header_parsed)
	{
		LOG_DEBUG("Iteration #" << x << "returning State::ReadingBody");
		return State::ParsingBody;
	}
	else
	{
		LOG_DEBUG("Iteration #" << x << "returning State::GeneratingResponse");
		return State::BuildingResponse;
	}
}

State HttpRequest::parse_body(std::vector<char>& buffer)
{
	_body_buffer.insert(_body_buffer.end(), std::make_move_iterator(buffer.begin()), std::make_move_iterator(buffer.end()));
	buffer.clear();
	std::string_view 	sv_buffer(_body_buffer.data(), _body_buffer.size());
	//extract filename if not yet extracted -> set file extracted true
	if (not _b_file_extracted)
	{
		_file.filename = _extract_filename(sv_buffer);
		if (_file.filename.empty())
		{
			LOG_ERROR("No filename extracted...");
		}
		else
		{
			_b_file_extracted = true;
			LOG_NOTICE("Filename extracted: " << _file.filename);
		}
	}
	//extract boundary if not yet extracted -> set boundary extracted true
	if (not _b_boundary_extracted)
	{
		std::string_view content_type = get_value("Content-Type").value_or("");
		_boundary = _extract_boundary(content_type);
		if (_boundary.empty())
		{
			LOG_ERROR("No boundary extracted... ");
		}
		else
		{
			LOG_NOTICE("Boundary extracted: " << _boundary);
			_b_boundary_extracted = true;
		}
	}
	//extract filedata if boundary is extracted
		//finish if end boundary found -> return State::GeneratingResponse
	if (_b_boundary_extracted)
	{
		size_t crln_pos = sv_buffer.find("\r\n\r\n");
		if (crln_pos == std::string::npos)
		{
			LOG_ERROR("No end CRLF found... ");
			return State::ParsingBody;
		}

		_boundary_end = "--" + _boundary + "--";
		auto body_data_start = (_body_buffer.begin() + crln_pos) + 4;
		auto body_data_end = std::search(body_data_start, _body_buffer.end(), _boundary_end.begin(), _boundary_end.end());
		if (body_data_end == _body_buffer.end())
		{
			LOG_ERROR("Ending boundary not present... ");
		}
		else
		{
			_file.data.assign(body_data_start, body_data_end - 2);
			_file.finished = false;
			_file.bytes_uploaded = 0;
			LOG_NOTICE("Ending boundary extracted: " << _boundary_end);
			return State::BuildingResponse;
		}
	}
	return State::ParsingBody;
}

//		extraction methods (private)
void	HttpRequest::_extract_request_line(std::istringstream 	&stream)
{
	std::string 				line;
	std::vector<std::string>	tokens;

	std::getline(stream, line);
	tokens = Utility::tokenize_string(line, " ");
	if (tokens.size() != 3)
		throw HttpException("HttpRequest: Missing method, location or protocol!");
	if (tokens[0] != "GET" && tokens[0] != "POST" && tokens[0] != "DELETE")
		throw HttpException("HttpRequest: Missing or incorrect request method!");

	//Extract method
	_method = tokens[0];

	//Extract type
	_type = REQUEST_TYPE.at(_method);

	//Extract uri
	if (tokens[1][0] != '/')
		throw HttpException("HttpRequest: URI not present!");
	_uri = tokens[1];

	//Extract filename, location and is_file_boolean
	std::filesystem::path p(_uri);
	_filename = p.filename().string();
	if (p.has_extension())
	{
		_b_file = true;
		_location = p.parent_path().string();
	}
	else
	{
		_location = "/" + p.stem().string();
	}

	//Extract protocol
	if (tokens[2] != "HTTP/1.1\r")
		throw HttpException("HttpRequest: Protocol not present!");
	_protocol = tokens[2];
}

void	HttpRequest::_extract_header_fields(std::string_view data_sv)
{
	std::istringstream 			stream(data_sv.data()); // Might be dangerous because sv is not guaranteed to be null-terminated
	std::string					line;

	_extract_request_line(stream);
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
				if (value.rfind('\r') != std::string::npos)
					value.pop_back();
				_header.emplace(key, value);
			}
	}
	_b_header_parsed = true;
}

std::filesystem::path HttpRequest::_extract_file_path(std::string_view filename)
{
	return std::filesystem::path(std::string(get_uri()) +  "/" +  std::string(filename));
}

std::string HttpRequest::_extract_filename(std::string_view body_buffer)
{
	size_t filename_begin = body_buffer.find("filename=\"");
	if (filename_begin == std::string::npos)
	{
		LOG_ERROR("Filename not found in body buffer");
		return {};
	}
	filename_begin += 10; // Move past 'filename="'
	size_t filename_end = body_buffer.find("\r\n", filename_begin);
	if (filename_end == std::string::npos)
	{
		LOG_ERROR("Malformed filename in body buffer");
		return {};
	}
	return std::string(&body_buffer[filename_begin], &body_buffer[filename_end - 1]);
}

std::string HttpRequest::_extract_boundary(std::string_view content_type)
{
	size_t boundary_begin = content_type.find("boundary=");
	if (boundary_begin == std::string::npos)
	{
		LOG_ERROR("Boundary not found in Content-Type header");
		return {};
	}
	boundary_begin += 9; // Move past 'boundary='
	std::string boundary(content_type.substr(boundary_begin));
	return boundary;
}

std::ostream & operator << (std::ostream &out, const HttpRequest &request)
{
	if (!request.get_method().empty())
		out << YELLOW << request.get_method() << " " << request.get_uri() << " " << request.get_protocol() << std::endl;
	for (const auto& [key, value] : request.get_headers())
		out << key <<  ":" << value << "\n";
	out << END << std::endl;
	return out;
}
