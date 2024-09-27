#include "HttpServer.hpp"
#include "meta.hpp"
#include <cwchar>
#include <cstring>
#include <fstream>

HttpServer::HttpServer() : _b_headers_complete(false), _b_body_complete(false), _current_state(State::ReadingHeaders)
{
	response.set_state(NOT_READY);
}

HttpServer::HttpServer(t_config &config) : _b_headers_complete(false), _b_body_complete(false), _current_state(State::ReadingHeaders), _config(config)
{
	response.set_state(NOT_READY);
}

HttpServer::~HttpServer()
{
	// LOG(RED << "DELETING HTTPSERVER!" << END);
}

HttpServer::HttpServer(const HttpServer &other) : _header_buffer(other._header_buffer), _body_buffer(other._body_buffer), _b_headers_complete(other._b_headers_complete), _b_body_complete(other._b_body_complete), _current_state(other._current_state)
{
	// LOG("HttpServer : copied for sock_fd: " << _socket.get_fd());
}


void	HttpServer::handle(std::vector<char> data)
{
	on_data_received(data);
}

void		HttpServer::on_data_received(std::vector<char> data)
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
			response.set_state(_cgi.poll());
			break;
		case State::ProcessingBody:
			response.set_state(build_body(response.get_path()));
			LOG_DEBUG("Processing body...");
			break;
	}
}

void		HttpServer::handle_headers(std::vector<char> data)
{
	static int iterations;
	LOG_DEBUG("handle_headers #" << iterations);
	iterations++;

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


void		HttpServer::handle_body(std::vector<char> data)
{
	static int it;
	LOG_DEBUG("handle_body #" << it);
	it++;

	std::string_view sv(_body_buffer.data(), _body_buffer.size());
	if (_body_buffer.size() == Utility::svtoi(request.get_value("Content-Length")))
	{
			_current_state = State::GeneratingResponse;
			LOG_ERROR("Generating response... ");
			generate_response();
			return;
	}
	else
	{
		_body_buffer.insert(_body_buffer.end(), data.begin(), data.end());
		LOG_ERROR("Body buffer size: " << _body_buffer.size());
		if (_body_buffer.size() == Utility::svtoi(request.get_value("Content-Length")))
		{
			LOG_ERROR("Generating response... ");
			generate_response();
			return;
		}
	}
}

void		HttpServer::generate_response()
{
	_body_buffer.push_back('\0');
	request.set_body(_body_buffer);
	auto handler = HandlerFactory::create_handler(request.get_type());
	response = handler->handle_request(request, _config);
	if (response.get_type() == ResponseType::CGI)
	{
		_current_state = State::ProcessingCGI;
		_cgi.start("sleep_echo_var");
	}
	else if (response.get_type() == ResponseType::FETCH_FILE)
	{
		_current_state = State::ProcessingBody;
		build_body(response.get_path());
	}
}

std::string	HttpServer::get_data()
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

bool		HttpServer::is_ready()
{
	return this->response.is_ready();
}

void 		HttpServer::poll_cgi()
{
	if (response.get_type() == ResponseType::CGI)
		response.set_state(_cgi.poll());
}

bool	HttpServer::build_body(std::string_view path)
{
	char 			buffer[1024];
	constexpr auto 	read_size 	= std::size_t(1024);
	auto 			file_stream = std::ifstream(path.data(), std::ios::binary);
	auto 			out 		= std::string();

	if (not file_stream)
	{
		response.set_status_code(400);
		response.set_status_mssg("Could not fetch file");
		response.set_state(READY);
		response.set_type(ResponseType::ERROR);
		response.set_streamcount(0);
		LOG_ERROR("Could not open file");
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
			response.set_status_code(200);
			response.set_state(READY);
			response.set_streamcount(0);
			return true;
		}
	}
	return false;
}

