#include <FileHandler.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

class ClientHandler;

FileHandler::FileHandler(File& file) : _file(file)
{
	LOG_DEBUG("Pop back b4" << _file.path);
	_file.path.pop_back();
	LOG_DEBUG("Pop back after" << _file.path);
	open_file();
}

void FileHandler::handle_file(short events)
{
	if (events & POLLIN)
	{
		read_file();
	}
}

void FileHandler::open_file()
{
	_file.fd = ::open(_file.path.c_str(), O_RDONLY);
	if (_file.fd < 0)
	{
		LOG_ERROR("Failed opening file '" << _file.path.c_str() << "' in order to handle the static file request");
		_file.is_open = false;
		return;
	}
	_file.is_open = true;
}

void	FileHandler::read_file()
{
		char 	buffer[FETCH_READ_SIZE];

		if (_file.is_open)
		{
			::memset(buffer, '\0', FETCH_READ_SIZE);
			int bytes_read = read(_file.fd, buffer, FETCH_READ_SIZE - 1);
			if (bytes_read < 0)
			{
				LOG_ERROR("Error while reading file...");
			}
			if (bytes_read > 0)
			{
				_file.streamcount += bytes_read;
				_file.data.insert(_file.data.end(), buffer, buffer + bytes_read);
			}
			if (bytes_read == 0)
			{
				_file.finished = true;
				_file.is_open = false;
				close(_file.fd);
				LOG_DEBUG("FileHandler is done...");
			}
		}
		else
		{
			LOG_ERROR("Attempting to read file that's not opened");
		}
}

bool	FileHandler::is_finished()
{
	return _file.finished;
}

File&	FileHandler::get_file()
{
	return _file;
}
