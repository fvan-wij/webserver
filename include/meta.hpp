#pragma once

#include "Logger.hpp"
#include <climits>
#include <vector>


#define POLL_TIMEOUT 100
#define LISTEN_BACKLOG 8

// Chunk sizes
#define CHUNK_SIZE 			4096 * 4

#define SOCKET_READ_SIZE 	CHUNK_SIZE
#define UPLOAD_CHUNK_SIZE 	CHUNK_SIZE
#define FETCH_READ_SIZE 	CHUNK_SIZE
#define PIPE_READ_SIZE 		PIPE_BUF

#define TIME_OUT			8000 // ms

#define RED 	"\x1B[31m"
#define GREEN 	"\x1B[32m"
#define YELLOW 	"\x1B[33m"
#define END		"\033[0m\t\t"

enum class State {
	ParsingHeaders,
	ParsingBody,
	ParsingChunkedBody,
	ProcessingRequest,
	ProcessingCGI,
	ProcessingFileIO,
	Ready,
};

typedef struct FileUpload 
{
	std::string			filename;
	std::string			path;
	std::vector<char> 	data;
	size_t				bytes_uploaded;
	bool				finished;
} FileUpload;

typedef struct File
{
	int					fd;
	std::string			name;
	std::string			path;
	std::vector<char> 	data;
	size_t				streamcount;
	bool				finished;
	bool				is_open;
} File;
