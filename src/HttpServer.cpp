#include "HttpServer.hpp"
#include "meta.hpp"
#include <cwchar>
#include <string>

HttpServer::HttpServer() : _b_headers_complete(false), _b_body_complete(false), _current_state(State::ReadingHeaders)
{
	response.set_state(NOT_READY);
	// request.set_header_parsed(false);
	// request.set_body_parsed(false);
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
	if (data.empty())
	{
		generate_response();
		LOG_WARNING("IT'S EMPTY NOW");
	}
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
	}
}

void		HttpServer::handle_headers(std::vector<char> data)
{
	static int iterations;
	LOG_DEBUG("handle_headers #" << iterations);
	iterations++;
	std::string_view str = data.data();
	size_t	header_size = str.find("\r\n\r\n");
	if (header_size != std::string::npos)
	{
		_header_buffer += str.substr(0, header_size);
		_b_headers_complete = true;
		_body_buffer.insert(_body_buffer.end(), data.begin() + (header_size + 4), data.end());
		request.parse_header(_header_buffer);
		LOG_INFO("==HEADER_BUFFER==\n" << _header_buffer);
		LOG_INFO("==BODY_BUFFER==\n" << _body_buffer.data());
		if (str.find("WebKitFormBoundary") != std::string::npos){
			size_t i = str.find("WebKitFormBoundary");
			_current_state = State::ReadingBody;
			handle_body(static_cast<std::vector<char>>(data.data()[i]));
			return;
		}
		else if ((header_size + 4) == str.length())
		{
			// _current_state = State::ReadingBody;
			_current_state = State::GeneratingResponse;
			_b_body_complete = true;
			generate_response();
		}
		std::string str = request.get_value("Content-Length");
		if (!str.empty())
		{
			try {
				size_t content_length = std::stoi(str);
				if (content_length == _body_buffer.size())
				{
					_current_state = State::GeneratingResponse;
					_b_body_complete = true;
					generate_response();
				}
				else
				{
					_current_state = State::ReadingBody;
					return;
				}
			}
			catch (std::invalid_argument &e){
				LOG_ERROR("Hmv'e" << e.what());
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
	static size_t len;
	LOG_DEBUG("handle_body #" << it);
	it++;

	len = _body_buffer.size();
	if (data.empty())
	{
		_current_state = State::GeneratingResponse;
		_b_body_complete = true;
		LOG_INFO("==HEADER_BUFFER==\n" << _header_buffer);
		LOG_INFO("==BODY_BUFFER==\n" << _body_buffer.data());
		request.parse_body(_body_buffer);
	} else
	{
		_body_buffer.insert(_body_buffer.end(), data.begin(), data.end());
	}

	if (_body_buffer.size() == len)
	{
		_current_state = State::GeneratingResponse;
		generate_response();
	}else if (data.size() < SOCKET_READ_SIZE)
	{
		_current_state = State::GeneratingResponse;
		generate_response();
	}
}

void		HttpServer::generate_response()
{
	auto handler = HandlerFactory::create_handler(request.get_type());
	response = handler->handle_request(request, TEST_CONFIG);
	if (response.get_type() == ResponseType::CGI)
	{
		_current_state = State::ProcessingCGI;
		_cgi.start("sleep_echo_var");
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

