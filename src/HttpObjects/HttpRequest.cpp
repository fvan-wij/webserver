#include "HttpRequest.hpp"
#include "Logger.hpp"
#include "Utility.hpp"

#include <algorithm>

HttpRequest::HttpRequest()
	: _b_header_parsed(false), _b_body_parsed(false),
	_b_file_extracted(false), _b_file_path_extracted(false),
	_b_boundary_extracted(false), _b_file_data_extracted(false),
	_b_chunk_size_extracted(false), _current_chunk_size(0),	_file({})
{

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

void HttpRequest::set_file_path(std::string_view path)
{
	_file.path = path.data();
}

State	HttpRequest::parse_header(std::vector<char>& buffer)
{
	std::string_view data_sv(buffer.data(), buffer.size());

	if (not _b_header_parsed)
	{
		size_t	header_end = data_sv.find("\r\n\r\n");
		if (header_end != std::string::npos)
		{
			_header_buffer += data_sv.substr(0, header_end);
			_extract_header_fields(_header_buffer);
			buffer.erase(buffer.begin(), buffer.begin() + header_end + 4);
		}
		else
		{
			_header_buffer.append(buffer.data(), buffer.size());
			buffer.clear();
			return State::ParsingHeaders;
		}
	}

	if (get_value("Transfer-Encoding") == "chunked")
	{
		LOG_DEBUG("It's a chunky boi!");
		return State::ParsingChunkedBody;
	}

	if (not buffer.empty() && _b_header_parsed)
	{
		return State::ParsingBody;
	}
	else
	{
		std::string_view cont_len = get_value("Content-Length").value_or("0");
		if (Utility::svtoi(cont_len) != 0 && not _b_body_parsed)
		{
			return State::ParsingBody;
		}
		return State::ProcessingRequest;
	}
}

void	HttpRequest::_extract_chunk_size(std::vector<char>& buffer)
{
	if (buffer.size() >= 2 && buffer[0] == '\r' && buffer[1] == '\n')
		buffer.erase(buffer.begin(), buffer.begin() + 2);
	std::string	str_buffer(buffer.begin(), buffer.end());
	size_t chunk_size_pos = str_buffer.find("\r\n");
	if (chunk_size_pos == std::string::npos)
	{
		_current_chunk_size = -1;
		return;
	}
	std::string	chunk_size_str(str_buffer.begin(), str_buffer.begin() + chunk_size_pos);
	// LOG_DEBUG("Found chunk string: " << chunk_size_str << ", pos: " << chunk_size_pos);
	_current_chunk_size = std::stoi(chunk_size_str, nullptr, 16);
	_b_chunk_size_extracted = true;
	buffer.erase(buffer.begin(), buffer.begin() + chunk_size_pos + 2);
}

State HttpRequest::parse_body_chunked(std::vector<char>& buffer)
{
	if (not _left_over.empty())
	{
		buffer.insert(buffer.begin(), _left_over.begin(), _left_over.end());
		_left_over.clear();
	}
	// Extract hexadecimal chunk size
	if (not _b_chunk_size_extracted)
	{
		_extract_chunk_size(buffer);
		if (_current_chunk_size == 0)
		{
			LOG_DEBUG("_body_buffer.size(): " << _body_buffer.size());
			_file.data = _body_buffer;
			exit(123);
			return State::ProcessingRequest;
		}
		else if (_current_chunk_size == -1)
		{
			_left_over.insert(_left_over.end(), buffer.begin(), buffer.end());
			buffer.clear();
			return State::ParsingChunkedBody;
		}
	}

	size_t	remaining = _current_chunk_size - _current_chunk.size();
	size_t	to_read = std::min(remaining, buffer.size());

	_current_chunk.insert(_current_chunk.end(), buffer.begin(), buffer.begin() + to_read);
	buffer.erase(buffer.begin(), buffer.begin() + to_read);

	// Reset chunk_extraction boolean in case there's another chunk
	if (_current_chunk.size() == (size_t)_current_chunk_size)
	{
		_b_chunk_size_extracted = false;
		_body_buffer.insert(_body_buffer.end(), _current_chunk.begin(), _current_chunk.end());
		if (buffer.size() >= 2 && buffer[0] == '\r' && buffer[1] == '\n')
			buffer.erase(buffer.begin(), buffer.begin() + 2);
		_current_chunk.clear();
	}
	return State::ParsingChunkedBody;
}

State HttpRequest::parse_body(std::vector<char>& buffer)
{
	LOG_NOTICE("Parsing body...");
	if (buffer.size() > Utility::svtoi(get_value("Content-Length").value_or("0")))
		throw HttpException(400, "Bad Request");

	_body_buffer.insert(_body_buffer.end(), std::make_move_iterator(buffer.begin()), std::make_move_iterator(buffer.end()));
	buffer.clear();
	std::string_view 	sv_buffer(_body_buffer.data(), _body_buffer.size());

	if (get_value("Content-Type") == "plain/text")
	{
		size_t crln_pos = sv_buffer.find("\r\n\r\n");
		if (crln_pos == std::string::npos)
		{
			LOG_ERROR("End CRLF found... ");
			return State::ProcessingRequest;
		}
	}

	//extract filename if not yet extracted -> set file extracted true
	if (not _b_file_extracted)
	{
		_file.name = _extract_filename(sv_buffer);
		if (_file.name.empty())
		{
			LOG_ERROR("No filename extracted...");
		}
		else
		{
			_b_file_extracted = true;
			LOG_NOTICE("Filename extracted: " << _file.name);
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
			_file.streamcount = 0;
			LOG_NOTICE("Ending boundary extracted: " << _boundary_end);
			return State::ProcessingRequest;
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
		throw HttpException(400, "Bad Request");
	if (tokens[0] != "GET" && tokens[0] != "POST" && tokens[0] != "DELETE")
		throw HttpException(405, "Method Not Allowed");

	//Extract method
	_method = tokens[0];

	//Extract type
	_type = REQUEST_TYPE.at(_method);

	//Extract uri
	if (tokens[1][0] != '/')
		throw HttpException(400, "URI not present!");
	_uri = tokens[1];

	//Extract location
	std::filesystem::path p(_uri);
	_location = p.parent_path().string();

	//Extract protocol
	if (tokens[2] != "HTTP/1.1\r")
		throw HttpException(505, "HTTP Version Not Supported");
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
