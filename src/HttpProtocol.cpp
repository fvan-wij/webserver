#include "HttpProtocol.hpp"
#include "meta.hpp"
#include <cwchar>
#include <string>

HttpProtocol::HttpProtocol() : _b_headers_complete(false), _b_body_complete(false), _current_state(State::ReadingHeaders)
{
	response.set_state(NOT_READY);
}

HttpProtocol::HttpProtocol(t_config &config) : _b_headers_complete(false), _b_body_complete(false), _current_state(State::ReadingHeaders), _config(config)
{
	response.set_state(NOT_READY);
}

HttpProtocol::~HttpProtocol()
{
	// LOG(RED << "DELETING HTTPSERVER!" << END);
}

HttpProtocol::HttpProtocol(const HttpProtocol &other) : _header_buffer(other._header_buffer), _body_buffer(other._body_buffer), _b_headers_complete(other._b_headers_complete), _b_body_complete(other._b_body_complete), _current_state(other._current_state)
{
	// LOG("HttpServer : copied for sock_fd: " << _socket.get_fd());
}


void	HttpProtocol::handle(std::vector<char> data)
{
	on_data_received(data);
}

void		HttpProtocol::on_data_received(std::vector<char> data)
{
	switch (_current_state)
	{
		case State::ReadingHeaders:
			handle_headers(data);
			break;
		case State::ReadingBody:
			handle_body(data);
			break;
		case State::GeneratingResponse:
			generate_response();
			break;
		case State::ProcessingCGI:
			break;
		case State::UploadingFile:
			break;
	}
}

void		HttpProtocol::handle_headers(std::vector<char> data)
{
	std::string_view str(data.data(), data.size());
	size_t	header_size = str.find("\r\n\r\n", 0);

	if (header_size != std::string::npos)
	{
		_header_buffer += str.substr(0, header_size);
		_b_headers_complete = true;
		_body_buffer.insert(_body_buffer.end(), data.begin() + (header_size + 4), data.end());
		request.parse_header(_header_buffer);
		std::string_view sv_body(_body_buffer.data(), _body_buffer.size());
		if (str.find("WebKitFormBoundary") != std::string::npos)
		{
			_current_state = State::ReadingBody;
			return;
		}
		else if ((header_size + 4) == str.length())
		{
			_current_state = State::GeneratingResponse;
			_b_body_complete = true;
			generate_response();
		}
		std::optional<std::string_view> val = request.get_value("Content-Length");
		if (val)
		{
			try {
				auto len = Utility::svtoi(val);
				if (len && len == _body_buffer.size())
				{
					_current_state = State::GeneratingResponse;
					_b_body_complete = true;
					generate_response();
					return;
				}
				else
				{
					_current_state = State::ReadingBody;
					return;
				}
			}
			catch (std::invalid_argument &e)
			{
				exit(123);
			}
		}
	}
	else {
		_header_buffer.append(data.data(), data.size());
	}
}


void		HttpProtocol::handle_body(std::vector<char> data)
{
	std::string_view sv(_body_buffer.data(), _body_buffer.size());
	if (_body_buffer.size() == Utility::svtoi(request.get_value("Content-Length")))
	{
			_current_state = State::GeneratingResponse;
			LOG_DEBUG("Generating response... ");
			generate_response();
			return;
	}
	else
	{
		_body_buffer.insert(_body_buffer.end(), data.begin(), data.end());
		if (_body_buffer.size() == Utility::svtoi(request.get_value("Content-Length")))
		{
			LOG_DEBUG("Generating response... ");
			generate_response();
			return;
		}
	}
}

void		HttpProtocol::generate_response()
{
	_body_buffer.push_back('\0');
	request.set_body(_body_buffer);
	auto handler = HandlerFactory::create_handler(request.get_type());
	response = handler->handle_request(request, _config);
	if (response.get_type() == ResponseType::CGI)
	{
		_current_state = State::ProcessingCGI;
	}
	else if (response.get_type() == ResponseType::UPLOAD)
	{
		_current_state = State::UploadingFile;
		parse_file_data(request.get_body(), _config, request.get_uri());
	}
}

void	HttpProtocol::start_cgi()
{
	_cgi.start("sleep_echo_var");
}

std::string	HttpProtocol::get_data()
{
	if (!response.is_ready())
	{
		WARNING("calling get_data() while not ready!");
	}
	if (response.get_type() == ResponseType::CGI)
	{
		std::string b = _cgi.get_buffer();
		response.append_body(b);
	}
	return response.to_string();
}

t_config	HttpProtocol::get_config()
{
	return (_config);
}

int	HttpProtocol::get_pipe_fd()
{
	return _cgi.get_pipe_fd();
}

bool		HttpProtocol::is_ready()
{
	return this->response.is_ready();
}

void 		HttpProtocol::poll_cgi()
{
	if (response.get_type() == ResponseType::CGI)
		response.set_state(_cgi.poll());
}

void	HttpProtocol::poll_upload()
{
	if (response.get_type() == ResponseType::UPLOAD)
		response.set_state(upload_chunk());
}

static std::string get_file_path(std::string_view root, std::string_view uri, std::string_view filename)
{
	std::string path = ".";

	return path + root.data() + uri.data() + "/" + filename.data();
}

static std::string get_filename(std::string_view body_buffer)
{
	size_t filename_begin = body_buffer.find("filename=\"") + 10;
	size_t filename_end = body_buffer.find("\r\n", filename_begin);
	std::string filename (&body_buffer[filename_begin], &body_buffer[filename_end - 1]);
	LOG_DEBUG("FILENAME IS: " << filename);
	return filename;
}

static std::string get_boundary(std::string_view content_type)
{
	size_t boundary_begin = content_type.find("boundary=") + 9;
	std::string boundary(&content_type[boundary_begin], content_type.end());
	LOG_DEBUG("Boundary: " << boundary);
	return boundary;
}

void	HttpProtocol::parse_file_data(std::vector<char> buffer, t_config& config, std::string_view uri)
{
	std::string_view 	sv_buffer(buffer.data(), buffer.size());
	_file.filename = get_filename(sv_buffer);
	_file.path = get_file_path(config.root, uri.data(), _file.filename);
	std::string 		boundary_end = "--" + get_boundary(request.get_value("Content-Type").value()) + "--";

	//Remove beginning till CRLN
	size_t crln_pos = sv_buffer.find("\r\n\r\n");
	buffer.erase(buffer.begin(), (buffer.begin() + crln_pos) + 4);
	//Remove ending boundary and everything after
	auto it_end_boundary = std::search(buffer.begin(), buffer.end(), boundary_end.begin(), boundary_end.end());
	if (it_end_boundary != buffer.end())
	{
		buffer.erase(it_end_boundary - 2, buffer.end());
		_file.data = buffer;
		_file.finished = false;
		_file.bytes_uploaded = 0;
	}
	else
	{
		LOG_ERROR("Ending boundary not present");
	}
}

static bool file_exists(std::string_view file_name)
{
	std::ifstream in_file(file_name.data());
	return in_file.good();  
}

bool	HttpProtocol::upload_chunk()
{
	size_t UPLOAD_CHUNK_SIZE = 4096;
	size_t bytes_left = _file.data.size() - _file.bytes_uploaded;
	size_t buffer_size = UPLOAD_CHUNK_SIZE;

	if (bytes_left < UPLOAD_CHUNK_SIZE)
	{
		buffer_size = bytes_left;
		LOG_INFO("bytes_left: " << bytes_left);
	}
	else if (bytes_left <= 0)
	{
		_file.finished = true;
		return true;
	}

	if (!file_exists(_file.path))
	{
		std::ofstream outfile(_file.path.data(), std::ios::binary);
			outfile.write(&_file.data[_file.bytes_uploaded], buffer_size);
	}
	else
	{
		std::ofstream outfile(_file.path.data(), std::ios::binary | std::ios::app);
		outfile.write(&_file.data[_file.bytes_uploaded], buffer_size);
	}
	_file.bytes_uploaded += buffer_size;
	return bytes_left <= 0;
}

HttpProtocol &HttpProtocol::operator=(const HttpProtocol &other)
{
	_header_buffer = other._header_buffer;
	_body_buffer = other._body_buffer;
	_b_headers_complete = other._b_headers_complete;
	_b_body_complete = other._b_body_complete;
	_current_state = other._current_state;
	return *this;
}
