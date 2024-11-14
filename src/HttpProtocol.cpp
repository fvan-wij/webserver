#include "HttpProtocol.hpp"
#include "HandlerFactory.hpp"

#include <cwchar>
#include <string>
#include <fstream>

HttpProtocol::HttpProtocol() : _state(State::ParsingHeaders)
{

}


HttpProtocol::HttpProtocol(Config config) : _state(State::ParsingHeaders), _config(config)

{

}

HttpProtocol::~HttpProtocol()
{

}

HttpProtocol::HttpProtocol(const HttpProtocol &other) : _state(other._state), _config(other._config)
{

}

/**
 * @brief Parses the chunk of incoming data.
 * With each parsing call, the data is being moved to the request object
 * and repeated until the incoming data buffer is empty.
 */
void	HttpProtocol::parse_data(std::vector<char>& data)
{
	while (not data.empty())
	{
		switch (_state)
		{
			case State::ParsingHeaders:
				{
					LOG_NOTICE("Parsing header...");
					_state = request.parse_header(data);
				}
				break;
			case State::ParsingBody:
				{
					LOG_NOTICE("Parsing body...");
					_state = request.parse_body(data);
				}
				break;
			default:
				break;
		}
	}
	if (_state == State::BuildingResponse)
	{
		generate_response();
	}
}

/**
 * @brief Spawns a GET, POST or DELETE handler which validates and builds the response according
 * to the given config
 */
void		HttpProtocol::generate_response()
{
	auto handler = HandlerFactory::create_handler(request.get_type());
	response = handler->handle_request(request, _config);
	switch (response.get_type())
	{
		case ResponseType::CGI:
			_state = State::ProcessingCGI;
			break;
		case ResponseType::Upload:
			_file = request.get_file_upload();
			_state = State::UploadingFile;
			break;
		case ResponseType::Fetch:
			_state = State::FetchingFile;
			break;
		default:
			break;
	}
}

/**
 * @brief Starts the CGI which spawns a child process that runs .py scripts.
 * Can be triggered by both POST and GET requests.
 * Trailing pathnames that follow the scriptname (i.e. '/hello.py/yeet/') should be added to PATH_INFO.
 */
void	HttpProtocol::start_cgi(char *envp[])
{
	std::vector<const char*> args;

	//To do: find /usr/bin/python3
	//Differentiate between Upload, FETCH or simply running a script

	std::string path = response.get_path();
	LOG_DEBUG("path: " << path);

	args.push_back("/usr/bin/python3");
	args.push_back(path.c_str());

	_cgi.start(args, envp);
}

/**
 * @brief
 * @return the response as a std::string
 */
std::string	HttpProtocol::get_data()
{
	if (response.get_type() == ResponseType::CGI)
	{
		std::string b = _cgi.get_buffer();
		response.set_body(b);
	}
	return response.to_string();
}

/**
 * @brief
 * @return config belonging to the HttpProtocol
 */
Config	HttpProtocol::get_config()
{
	return (_config);
}

/**
 * @brief
 * @return pipe of CGI
 */
int	HttpProtocol::get_pipe_fd()
{
	return _cgi.get_pipe_fd();
}

/**
 * @brief
 * @return state of the protocol (ParsingHeaders, ParsingBody, BuildingResponse, ProcessingCGI, UploadingFile, FetchingFile, Ready)
 */
State HttpProtocol::get_state()
{
	return _state;
}

void		HttpProtocol::set_config(Config config)
{
	_config = config;
}

/**
 * @brief
 * @return true if response is ready to be sent to the client
 */
bool	HttpProtocol::is_ready()
{
	return response.is_ready();
}

/**
 * @brief
 * @return true if CGI is running
 */
bool	HttpProtocol::is_cgi_running()
{
	return _cgi.is_running();
}

/**
 * @brief Polls CGI and sets the response state to READY or NOT_READY
 */
void 		HttpProtocol::poll_cgi()
{
	response.set_state(_cgi.poll());
}

/**
 * @brief Polls file upload by uploading UPLOAD_CHUNK_SIZE bytes of data and setting the response state to READY or NOT_READY
 */
void	HttpProtocol::poll_upload()
{
	response.set_state(upload_chunk());
}

/**
 * @brief Polls fetch by reading FETCH_READ_SIZE bytes of a file and setting the response state to READY or NOT_READY
 */
void	HttpProtocol::poll_fetch()
{

	response.set_state(fetch_file(response.get_path()));
}

static bool file_exists(std::string_view file_name)
{
	std::ifstream in_file(file_name.data());
	return in_file.good();
}

/**
 * @brief Writes UPLOAD_CHUNK_SIZE bytes of data to location '_file.path' on the server.
 * @return true if finished.
 */
bool	HttpProtocol::upload_chunk()
{
	size_t bytes_left = _file.data.size() - _file.bytes_uploaded;
	size_t buffer_size = UPLOAD_CHUNK_SIZE;

	if (bytes_left < UPLOAD_CHUNK_SIZE)
	{
		buffer_size = bytes_left;
	}
	if (bytes_left <= 0)
	{
		_file.finished = true;
		return true;
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
	return bytes_left <= 0;
}

/**
 * @brief Sets the error code and message of a response and type to ResponseType::Error
 */
void	HttpProtocol::build_error_response(int error_code, std::string_view message)
{
	response.set_status_code(error_code);
	response.set_status_mssg(message.data());
	std::string mssg = "\r\n<h1>" + std::to_string(response.get_status_code()) + " " + response.get_status_mssg() + "</h1>\r\n";
	response.set_body(mssg);
	response.set_state(READY);
	response.set_type(ResponseType::Error);
}

/**
 * @brief Opens file on given path and appends FETCH_READ_SIZE bytes of a file to the body of the response.
 * @return true if finished.
 */
bool HttpProtocol::fetch_file(std::string_view path)
{
	char 			buffer[FETCH_READ_SIZE];
	auto 			file_stream = std::ifstream(path.data(), std::ios::binary);
	auto 			out 		= std::string();

	LOG_DEBUG("Fetch file on " << path);
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
		file_stream.read(&buffer[0], FETCH_READ_SIZE);
		std::streamsize bytes = file_stream.gcount();
		response.update_streamcount(bytes);
		std::vector<char> data(buffer, buffer + bytes);
		std::string str(data.begin(), data.end());
		response.append_body(str);
		if (bytes < 1024)
		{
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
	_state = other._state;
	return *this;
}
