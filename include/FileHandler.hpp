#pragma once

#include <Socket.hpp>
#include <ConnectionManager.hpp>

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

        // 		Exceptions
		class FileHandlerException : public std::exception
		{
			protected:
				std::string     message;
			public:
				FileHandlerException(const std::string& msg) : message(msg){};
				const char* what() const noexcept override
				{
					return message.c_str();
				}
		};

		class OpeningFileException : public FileHandlerException
		{
			public:
				OpeningFileException(const std::string& msg)
					: FileHandlerException("OpeningFileException: " + msg) {}
		};

		class CreatingFileException : public FileHandlerException
		{
			public:
				CreatingFileException(const std::string& msg)
					: FileHandlerException("CreatingFileException: " + msg) {}
		};

		class ReadingFileException : public FileHandlerException
		{
			public:
				ReadingFileException(const std::string& msg)
					: FileHandlerException("Reading exception: " + msg) {}
		};

		class WritingFileException : public FileHandlerException
		{
			public:
				WritingFileException(const std::string& msg)
					: FileHandlerException("WritingException: " + msg) {}
		};

		class PermissionException : public FileHandlerException
		{
			public:
				PermissionException(const std::string& msg)
					: FileHandlerException("PermissionException: " + msg) {}
		};

	private:
		File			&_file;
		ResponseType	_type;

		void	_open_file();
		void	_create_file();
		void	_read_file();
		void	_write_file();
};
