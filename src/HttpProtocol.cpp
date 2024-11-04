#include "HttpProtocol.hpp"
#include "HandlerFactory.hpp"
#include <cwchar>
#include <string>

HttpProtocol::HttpProtocol() : _b_headers_complete(false), _b_body_complete(false), _state(State::ReadingHeaders)
{
	response.set_state(NOT_READY);
}

HttpProtocol::HttpProtocol(t_config &config) : _b_headers_complete(false), _b_body_complete(false), _state(State::ReadingHeaders), _config(config)
{
	response.set_state(NOT_READY);
}

HttpProtocol::~HttpProtocol()
{

}

HttpProtocol::HttpProtocol(const HttpProtocol &other) : _b_headers_complete(other._b_headers_complete), _b_body_complete(other._b_body_complete), _state(other._state)
{

}

void	HttpProtocol::parse_data(std::vector<char>& data)
{
	int iterations = 0;
	while (not data.empty())
	{
		switch (_state)
		{
			case State::ReadingHeaders:
				{
					LOG_NOTICE("Parsing header...");
					_state = request.parse_header(data);
				}
				break;
			case State::ReadingBody:
				{
					LOG_NOTICE("Parsing body...");
					_state = request.parse_body(data);
				}
				break;
		}

		iterations++;
		LOG_DEBUG("parse_data iteration #" << iterations << ", state: " << int(_state));
		LOG_DEBUG("data left: " << data.size());
		if (data.size() != 0)
			LOG_DEBUG("data: " << data.data());
	}
	if (_state == State::GeneratingResponse)
	{
		LOG_NOTICE("Finished reading/parsing, on to generating a response!");
		generate_response();
	}
}

void	HttpProtocol::handle(std::vector<char>& data)
{
	parse_data(data);
}

void		HttpProtocol::generate_response()
{
	LOG_DEBUG("HAHA");
	// _body_buffer.push_back('\0');
	// request.set_body(_body_buffer);
	auto handler = HandlerFactory::create_handler(request.get_type());
	response = handler->handle_request(request, _config);
	if (response.get_type() == ResponseType::CGI)
	{
		_state = State::ProcessingCGI;
	}
	else if (response.get_type() == ResponseType::UPLOAD)
	{
		_state = State::UploadingFile;
		// parse_file_data(request.get_body_buffer(), _config, request.get_uri());
	}
	else if (response.get_type() == ResponseType::FETCH_FILE)
	{
		_state = State::FetchingFile;
	}
}

//CGI should be able to run any .py file
//CGI can be triggered by both POST and GET requests
//CGI can accept uploaded files and configure where they should be saved
//Trailing pathnames that follow the scriptname should be added to PATH_INFO

void	HttpProtocol::start_cgi(char *envp[])
{
	std::vector<const char*> args;

	//To do: find /usr/bin/python3
	//Differentiate between UPLOAD, FETCH or simply running a script

	std::string path = response.get_path();
	LOG_DEBUG("path: " << path);

	args.push_back("/usr/bin/python3");
	args.push_back(path.c_str());

	_cgi.start(args, envp);
}

std::string	HttpProtocol::get_data()
{
	if (response.get_type() == ResponseType::CGI)
	{
		std::string b = _cgi.get_buffer();
		response.set_body(b);
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

State HttpProtocol::get_state()
{
	return _state;
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

void	HttpProtocol::poll_fetch()
{
	if (response.get_type() == ResponseType::FETCH_FILE)
		response.set_state(fetch_file(response.get_path()));
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
	}
	if (!file_exists(_file.path))
	{
		std::ofstream outfile(_file.path, std::ios::binary);
			outfile.write(&_file.data[_file.bytes_uploaded], buffer_size);
	}
	else
	{
		std::ofstream outfile(_file.path, std::ios::binary | std::ios::app);
		outfile.write(&_file.data[_file.bytes_uploaded], buffer_size);
	}
	_file.bytes_uploaded += buffer_size;
	if (bytes_left <= 0)
	{
		_file.finished = true;
		return true;
	}
	return bytes_left <= 0;
}

void	HttpProtocol::build_error_response(int error_code, std::string_view message)
{
	response.set_status_code(error_code);
	response.set_status_mssg(message.data());
	std::string mssg = "\r\n<h1>" + std::to_string(response.get_status_code()) + " " + response.get_status_mssg() + "</h1>\r\n";
	response.set_body(mssg);
	response.set_state(READY);
	response.set_type(ResponseType::ERROR);
}

bool HttpProtocol::fetch_file(std::string_view path)
{
	char 			buffer[1024];
	constexpr auto 	read_size 	= std::size_t(1024);
	auto 			file_stream = std::ifstream(path.data(), std::ios::binary);
	auto 			out 		= std::string();

	if (response.get_streamcount() == 0)
		LOG_DEBUG("Fetching " << path.data());
	if (not file_stream)
	{
		build_error_response(404, "Not Found - The server cannot find the requested resource");
		response.set_streamcount(0);
		LOG_ERROR("Could not open file");
		return true;
	}
	else
	{
		file_stream.seekg(response.get_streamcount());
		file_stream.read(&buffer[0], read_size);
		std::streamsize bytes = file_stream.gcount();
		response.update_streamcount(bytes);
		std::vector<char> data(buffer, buffer + bytes);
		std::string str(data.begin(), data.end());
		response.append_body(str);
		if (bytes < 1024)
		{
			LOG_DEBUG("Finished... size: " << response.get_body().size());
			response.set_status_code(200);
			response.set_state(READY);
			response.set_streamcount(0);
			return true;
		}
	}
	return false;
}

HttpProtocol &HttpProtocol::operator=(const HttpProtocol &other)
{
	_b_headers_complete = other._b_headers_complete;
	_b_body_complete = other._b_body_complete;
	_state = other._state;
	return *this;
}
