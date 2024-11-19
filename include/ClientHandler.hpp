#pragma once

#include <string>
#include <vector>
#include <Socket.hpp>
#include <ConnectionManager.hpp>
#include <FileHandler.hpp>

class ClientHandler {
	public:
		ClientHandler(ConnectionManager& cm, Socket socket, std::vector<Config>& configs);
		ClientHandler(const ClientHandler &) = default;
		ClientHandler &operator=(const ClientHandler &) = default;
		~ClientHandler() = default;

								// Methods
		void					handle_request(short events);

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

		bool					_handle_incoming_data();
		void					_handle_outgoing_data();
		void					_parse(std::vector<char>& data);
		bool					_send_response(ResponseType type);
		void 					_poll_file_handler();
		void					_process_request();
		void					_add_file_handler();
};
