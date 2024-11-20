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
	_file.fd = ::open(_file.path.c_str(), O_RDONLY);
	if (_file.fd < 0)
	{
		LOG_ERROR("Failed opening file '" << _file.path.c_str() << "' in order to handle the static file request");
		_file.is_open = false;
		return;
	}
	_file.is_open = true;
}

void FileHandler::_create_file()
{
	_file.path += "/" + _file.name;
	_file.fd = ::open(_file.path.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0644);
	if (_file.fd < 0)
	{
		LOG_ERROR("Failed creating file '" << _file.path.c_str() << "' in order to handle the file upload");
		_file.is_open = false;
		return;
	}
	_file.is_open = true;
}

void	FileHandler::_read_file()
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


/**
 * @brief Writes UPLOAD_CHUNK_SIZE bytes of data to location '_file.path' on the server.
 * @return true if finished.
 */
void	FileHandler::_write_file()
{
	size_t bytes_left = _file.data.size() - _file.streamcount;
	size_t buffer_size = UPLOAD_CHUNK_SIZE;

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
		LOG_ERROR("Trying to write to non-existing file!");
	}
	_file.streamcount += buffer_size;
	_file.finished = bytes_left <= 0;
}

