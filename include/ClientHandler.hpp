#pragma once

#include <CgiHandler.hpp>
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
		ClientHandler(ConnectionManager& cm, Socket socket, std::vector<Config>& configs, int16_t port, char *envp[]);
		ClientHandler(const ClientHandler &) = delete;
		ClientHandler &operator=(const ClientHandler &) = delete;
		~ClientHandler() = default;

											// Methods
		void								handle_request(short revents);

											// Getters
		std::vector<Config>&				get_configs(){return _configs;};
		Socket&								get_socket(){return _socket;};
		uint16_t							get_port(){return _port;};
		ConnectionManager&					get_connection_manager(){return _connection_manager;};
		void								init_timer(){_timer.reset();};


	private:
		std::string							_response_data;
		std::vector<Config>					_configs;
		// CGI 								_cgi;
		Config								_config;
		Socket								_socket;
		ConnectionManager					&_connection_manager;
		CgiHandler 							*_cgi_handler;
		FileHandler					 		*_file_handler;
		File								_file;
		State								_state;
		Timer								_timer;
		bool								_timed_out;
		uint16_t							_port;

		HttpRequest							_request;
		HttpResponse						_response;

		char 								**_envp;

		void								_handle_incoming_data();
		void								_handle_outgoing_data();
		void								_poll_cgi();
		std::optional<std::string> 			_retrieve_error_path(int error_code, Config &config);
		void								_build_error_response(int status_code, const std::string& message, std::optional<std::string> error_path);
		void								_build_redirection_response(int status_code, const std::string& message);
		void								_process_request();
		void								_parse(std::vector<char>& data);
		void								_send_response();
		void 								_poll_file_handler();
		Config								_resolve_config(std::optional<std::string_view> host);
		void								_add_file_handler(ResponseType type);
		void 								_add_cgi_handler();
		void								_poll_timeout_timer();
		void								_close_connection();

};
