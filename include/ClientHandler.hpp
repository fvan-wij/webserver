#pragma once

#include <string>
#include <vector>
#include <Socket.hpp>
#include <ConnectionManager.hpp>
#include <FileHandler.hpp>

class ClientHandler {
	public:
		ClientHandler(ConnectionManager &cm, Socket socket, std::vector<Config> &configs);
		ClientHandler(const ClientHandler &) = default;
		ClientHandler &operator=(const ClientHandler &) = default;
		~ClientHandler() = default;

								// Methods
		void					handle_request(short events);
		void					parse(std::vector<char>& data);

								// Getters
		std::vector<Config>&	get_configs(){return _configs;};
		Socket&					get_socket(){return _socket;};
		ConnectionManager&		get_connection_manager(){return _connection_manager;};

		HttpRequest				request;
		HttpResponse			response;

	private:
		std::string							_response_data;
		std::vector<Config>					_configs;
		Socket								_socket;
		ConnectionManager					&_connection_manager;
		FileHandler					 		*_file_handler;
		State								_state;
};
