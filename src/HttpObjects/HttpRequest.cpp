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

void HttpRequest::set_file_upload_path(std::string_view root)
{
	_file.filename.insert(0, "/");
	_file.path = root.data() + _file.filename;
}

State	HttpRequest::parse_header(std::vector<char>& data)
{
	std::string_view data_sv(data.data(), data.size());
	static int x;

	x++;
	LOG_DEBUG("Incoming datachunk on iteration " << x << " = " << data.size());
	if (not _b_header_parsed)
	{
		size_t	header_end = data_sv.find("\r\n\r\n", 0); //0 can be removed, right. Right???
		if (header_end != std::string::npos)
		{
			_header_buffer += data_sv.substr(0, header_end);
			_extract_header_fields(data_sv);
			_b_header_parsed = true;
			data.erase(data.begin(), data.begin() + header_end + 4);
			LOG_DEBUG("Iteration #" << x << "_b_header_parsed = true");
		}
		else
		{
			_header_buffer.append(data.data(), data.size());
			data.clear();
			LOG_DEBUG("Iteration #" << x << "returning State::ReadingHeaders");
			return State::ReadingHeaders;
		}
	}
	if (not data.empty() && _b_header_parsed)
	{
		LOG_DEBUG("Iteration #" << x << "returning State::ReadingBody");
		return State::ReadingBody;
	}
	else
	{
		LOG_DEBUG("Iteration #" << x << "returning State::GeneratingResponse");
		return State::GeneratingResponse;
	}
}

State HttpRequest::parse_body(std::vector<char>& data)
{
	_body_buffer.insert(_body_buffer.end(), std::make_move_iterator(data.begin()), std::make_move_iterator(data.end()));
	data.clear();
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
			return State::ReadingBody;
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
			return State::GeneratingResponse;
		}
	}
	return State::ReadingBody;
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
	_method = tokens[0];
	_type = REQUEST_TYPE.at(_method);

	if (tokens[1][0] != '/')
		throw HttpException("HttpRequest: URI not present!");
	_uri = tokens[1];
	if (_uri.rfind('.') != std::string::npos)
	{
		_b_file = true;
		_filename = _uri.substr(_uri.rfind('/'), _uri.length());
		if (std::count(_uri.begin(), _uri.end(), '/') == 1)
			_location = "/";
		else
			_location = _uri.substr(0, _uri.find("/", 1));
	}
	else 
	{
		_location = _uri.substr(0, _uri.find("/", 1));
		_b_file = false;
	}
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
				_header.emplace(key, value);
			}
	}
	_b_header_parsed = true;
}

std::string HttpRequest::_extract_file_path(std::string_view filename)
{
	return get_uri() + "/" + filename.data();
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
	// LOG_DEBUG("pos of /r " << boundary.find('\r'));
	int pos_r = boundary.find('\r');
	boundary = boundary.substr(0, pos_r);

	int x = 0;
	for (auto it : boundary)
	{
		LOG_DEBUG("Char: " << it << " , ascii: " << (int)it << ", " << x);
		x++;
	}
	if (boundary.find("WebKitFormBoundary") == std::string::npos)
	{
		return boundary;
	}
	else //Perhaps remove this and only focus on chrome requests
	{
		// boundary.pop_back(); // For some reason, there's an extra null terminator that fucks up string manipulation whenever I'm dealing with a webkit boundary (!????)
		return boundary;
	}
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
