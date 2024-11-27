#include <FileHandler.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <Utility.hpp>

class ClientHandler;

FileHandler::FileHandler(File& file, ResponseType type) : _file(file), _type(type)
{
	switch (type)
	{
		case ResponseType::Fetch:
			_open_file();
			break;
		case ResponseType::Error:
			_open_file();
			break;
		case ResponseType::Upload:
			_create_file();
			break;
		default:
			break;
	}
}

void FileHandler::handle_file(short events)
{
	if (events & POLLIN)
	{
		_read_file();
	}
	if (events & POLLOUT)
	{
		_write_file();
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

void FileHandler::_open_file()
{
	if (access(_file.path.c_str(), F_OK) == -1)
	{
		throw (PermissionException(_file.path + " doesn't exist"));
	}
	if (access(_file.path.c_str(), R_OK) == -1)
	{
		throw (PermissionException(_file.path + " has no reading permissions"));
	}
	_file.fd = ::open(_file.path.c_str(), O_RDONLY);
	if (_file.fd < 0)
	{
		_file.is_open = false;
		throw (OpeningFileException(_file.path + " couldn't open file"));
	}
	_file.is_open = true;
	LOG_DEBUG("Opened file successfully");
}

void FileHandler::_create_file()
{
	_file.path += "/" + _file.name;
	_file.fd = ::open(_file.path.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0644);
	if (_file.fd < 0)
	{
		_file.is_open = false;
		throw (CreatingFileException(_file.path + " could not be created in order to handle the file upload"));
	}
	_file.is_open = true;
}

void	FileHandler::_read_file()
{
		char 	buffer[FETCH_READ_SIZE];

		if (access(_file.path.c_str(), R_OK) == -1)
		{
			throw (PermissionException(_file.path + " doesn't have read permissions!"));
		}
		if (_file.is_open)
		{
			::memset(buffer, '\0', FETCH_READ_SIZE);
			int bytes_read = read(_file.fd, buffer, FETCH_READ_SIZE - 1);
			if (bytes_read < 0)
			{
				throw (ReadingFileException(_file.path));
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
			throw (ReadingFileException(_file.path + "'s filedescriptor is not opened!"));
		}
}


/**
 * @brief Writes UPLOAD_CHUNK_SIZE bytes of data to location '_file.path' on the server.
 * @return true if finished.
 */
void	FileHandler::_write_file()
{
	size_t bytes_left = _file.data.size() - _file.streamcount;
	size_t buffer_size = UPLOAD_CHUNK_SIZE;

	if (access(_file.path.c_str(), W_OK) == -1)
	{
		throw (PermissionException(_file.path + " doesn't have write permissions!"));
	}
	if (bytes_left < UPLOAD_CHUNK_SIZE)
	{
		buffer_size = bytes_left;
	}
	if (bytes_left <= 0)
	{
		_file.finished = true;
	}
	if (Utility::file_exists(_file.path))
	{
		write(_file.fd, &_file.data[_file.streamcount], buffer_size);
	}
	else
	{
		throw (WritingFileException(_file.path + " appears to be a a non-existing file!"));
	}
	_file.streamcount += buffer_size;
	_file.finished = bytes_left <= 0;
}

