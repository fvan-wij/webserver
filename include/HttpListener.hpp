#pragma once

#include <vector>
#include <Config.hpp>
#include <Socket.hpp>
#include <ConnectionManager.hpp>

class HttpListener {
	public:
		HttpListener(uint16_t port, ConnectionManager &cm);
		HttpListener(const HttpListener &) = default;
		HttpListener &operator=(const HttpListener &) = default;
		~HttpListener() = default;

		void				add_config(Config config);

		std::vector<Config>	get_configs();
		uint16_t			get_port();
		Socket				get_socket();

		void				listen_handle();

	private:
		std::vector<Config>	_configs;
		uint16_t			_port;
		Socket				_socket;
		ConnectionManager	&_connection_manager;
};