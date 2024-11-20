#pragma once

#include <Socket.hpp>
#include <ConnectionManager.hpp>

class ClientHandler;

class FileHandler {
	public:
		FileHandler(File& file, ResponseType type);
		FileHandler(const FileHandler &) = default;
		FileHandler &operator=(const FileHandler &) = default;
		~FileHandler() = default;

		void	handle_file(short events);
		bool	is_finished();
		File&	get_file();
		int		get_fd() {return _file.fd;};


	private:
		File			&_file;
		ResponseType	_type;

		void	_open_file();
		void	_create_file();
		void	_read_file();
		void	_write_file();
};
