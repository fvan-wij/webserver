#include "HttpProtocol.hpp"
#include "HandlerFactory.hpp"
#include <cwchar>
#include <string>
#include <fstream>

HttpProtocol::HttpProtocol() : _b_headers_complete(false), _b_body_complete(false), _state(State::ParsingHeaders)
{

}


HttpProtocol::HttpProtocol(Config &config) : _b_headers_complete(false), _b_body_complete(false), _current_state(State::ParsingHeaders), _config(config)

{

}

HttpProtocol::~HttpProtocol()
{

}

HttpProtocol::HttpProtocol(const HttpProtocol &other) : _b_headers_complete(other._b_headers_complete), _b_body_complete(other._b_body_complete), _state(other._state)
{

}

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
		}
	}
	if (_state == State::BuildingResponse)
	{
		LOG_NOTICE("Finished reading/parsing, on to generating a response!");
		LOG_DEBUG("Size of body: " << request.get_body_buffer().size());
		LOG_DEBUG("Content-Length: " << request.get_value("Content-Length").value_or("0"));
		generate_response();
	}
}

void	HttpProtocol::handle(std::vector<char>& data)
{
	parse_data(data);
}

void		HttpProtocol::generate_response()
{
	auto handler = HandlerFactory::create_handler(request.get_type());
	response = handler->handle_request(request, _config);
	if (response.get_type() == ResponseType::CGI)
	{
		_state = State::ProcessingCGI;
	}
	else if (response.get_type() == ResponseType::Upload)
	{
		_file = request.get_file_upload();
		_state = State::UploadingFile;
	}
	else if (response.get_type() == ResponseType::Fetch)
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
	//Differentiate between Upload, FETCH or simply running a script

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

Config	HttpProtocol::get_config()
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
	return response.is_ready();
}

void 		HttpProtocol::poll_cgi()
{
	response.set_state(_cgi.poll());
}

void	HttpProtocol::poll_upload()
{
	response.set_state(upload_chunk());
}

void	HttpProtocol::poll_fetch()
{

	response.set_state(fetch_file(response.get_path()));
}

static std::string get_file_path(std::string_view root, std::string_view uri, std::string_view filename)
{
	std::string path = ".";

	return path + root.data() + uri.data() + "/" + filename.data();
}

static std::string get_filename(std::string_view body_buffer)
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

static std::string get_boundary(std::string_view content_type)
{
	size_t boundary_begin = content_type.find("boundary=");
	if (boundary_begin == std::string::npos)
	{
		LOG_ERROR("Boundary not found in Content-Type header");
		return {};
	}
	boundary_begin += 9; // Move past 'boundary='
	std::string boundary(content_type.substr(boundary_begin));
	if (boundary.find("WebKitFormBoundary") == std::string::npos)
	{
		return boundary;
	}
	else
	{
		boundary.pop_back(); // For some reason, there's an extra null terminator that fucks up string manipulation whenever I'm dealing with a webkit boundary (!????)
		return boundary;
	}
}

void	HttpProtocol::parse_file_data(std::vector<char> buffer, Config& config, std::string_view uri)
{
	std::string_view 	sv_buffer(buffer.data(), buffer.size());

	_file.filename = get_filename(sv_buffer);
	if (_file.filename.empty())
	{
		LOG_ERROR("No filename extracted... aborting parse_file_data()");
		return;
	}
	_file.path = get_file_path(config.root, uri.data(), _file.filename);

	std::string_view content_type = request.get_value("Content-Type").value_or("");
	std::string boundary = get_boundary(content_type);
	if (boundary.empty())
	{
		LOG_ERROR("No boundary extracted... aborting parse_file_data()");
		return;
	}
	std::string	boundary_end = "--" + boundary + "--";
	size_t crln_pos = sv_buffer.find("\r\n\r\n");
	if (crln_pos == std::string::npos)
	{
		LOG_ERROR("No CRLF found... aborting parse_file_data()");
		return;
	}

	auto body_data_start = (buffer.begin() + crln_pos) + 4;
	auto body_data_end = std::search(body_data_start, buffer.end(), boundary_end.begin(), boundary_end.end());

	if (body_data_end == buffer.end())
	{
		LOG_ERROR("Ending boundary not present... aborting parse_file_data()");
		return;
	}
	else
	{
		_file.data.assign(body_data_start, body_data_end - 2);
		_file.finished = false;
		_file.bytes_uploaded = 0;
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

void	HttpProtocol::build_error_response(int error_code, std::string_view message)
{
	response.set_status_code(error_code);
	response.set_status_mssg(message.data());
	std::string mssg = "\r\n<h1>" + std::to_string(response.get_status_code()) + " " + response.get_status_mssg() + "</h1>\r\n";
	response.set_body(mssg);
	response.set_state(READY);
	response.set_type(ResponseType::Error);
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
