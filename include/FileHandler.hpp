#pragma once

#include <Socket.hpp>
#include <ConnectionManager.hpp>
#include <HttpResponse.hpp>
#include <meta.hpp>

class ClientHandler;

class FileHandler {
	public:
        // 		Constructors
		FileHandler(File& file, ResponseType type);
		FileHandler(const FileHandler &) = default;
		FileHandler &operator=(const FileHandler &) = default;
		~FileHandler() = default;

        // 		Methods
		void	handle_file(short events);
		bool	is_finished();
		int		get_fd() {return _file.fd;};
		File&	get_file();

	private:
		File			&_file;
		ResponseType	_type;

		void	_open_file();
		void	_create_file();
		void	_read_file();
		void	_write_file();
};
