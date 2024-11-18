#pragma once

#include <Socket.hpp>
#include <ConnectionManager.hpp>

class ClientHandler;

class FileHandler {
	public:
		FileHandler(File& file);
		FileHandler(const FileHandler &) = default;
		FileHandler &operator=(const FileHandler &) = default;
		~FileHandler() = default;

		void	handle_file(short events);
		void	open_file();
		void	read_file();
		bool	is_finished();
		File&	get_file();
		int		get_fd() {return _file.fd;};

	private:
		File	&_file;
};
