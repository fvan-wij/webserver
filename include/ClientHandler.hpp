#pragma once
#include <string>
#include <vector>
#include <Socket.hpp>
#include <ConnectionManager.hpp>
#include <FileHandler.hpp>
#include <HttpRequest.hpp>
#include <HttpResponse.hpp>
#include <Timer.hpp>

class ClientHandler {
	public:
		ClientHandler(ConnectionManager& cm, Socket socket, std::vector<Config>& configs);
		ClientHandler(const ClientHandler &) = default;
		ClientHandler &operator=(const ClientHandler &) = default;
		~ClientHandler() = default;

											// Methods
		void								handle_request(short revents);

											// Getters
		std::vector<Config>&				get_configs(){return _configs;};
		Socket&								get_socket(){return _socket;};
		ConnectionManager&					get_connection_manager(){return _connection_manager;};

		HttpRequest							request;
		HttpResponse						response;

	private:
		std::string							_response_data;
		std::vector<Config>					_configs;
		Config								_config;
		Socket								_socket;
		ConnectionManager					&_connection_manager;
		FileHandler					 		*_file_handler;
		State								_state;
		Timer								_timer;
		bool								_timed_out;

		void								_handle_incoming_data();
		void								_handle_outgoing_data();
		std::optional<std::string> 			_retrieve_error_path(int error_code, Config &config);
		void								_build_error_response(int status_code, const std::string& message);
		void								_build_redirection_response(int status_code, const std::string& message);
		void								_process_request();
		void								_parse(std::vector<char>& data);
		void								_send_response();
		void								_send_response(ResponseType type);
		void 								_poll_file_handler();
		Config								_resolve_config(std::optional<std::string_view> host);
		void								_add_file_handler(ResponseType type);
		void								_poll_timeout_timer();
		void								_close_connection();

};
