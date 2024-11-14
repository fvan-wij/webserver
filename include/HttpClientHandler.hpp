#pragma once

#include <string>
#include <vector>
#include <Socket.hpp>
#include <ConnectionManager.hpp>

class HttpClientHandler {
	public:
		HttpClientHandler(ConnectionManager &cm, Socket socket, std::vector<Config> configs);
		HttpClientHandler(const HttpClientHandler &) = default;
		HttpClientHandler &operator=(const HttpClientHandler &) = default;
		~HttpClientHandler() = default;

		void	handle_request(short events);

	private:
		std::optional<std::vector<char>>	_request;
		std::string					_response;
		std::vector<Config>			_configs;
		Socket						_socket;
		ConnectionManager			&_connection_manager;
		HttpProtocol				_protocol;
};