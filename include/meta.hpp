#pragma once

#include "Logger.hpp"
#include <vector>


#define POLL_TIMEOUT 100
#define LISTEN_BACKLOG 8

#define SOCKET_READ_SIZE 1024

#define PIPE_READ_SIZE 1024


#define UNUSED(x) (void)(x)
#define WARNING(message) \
	do { \
		LOG_ERROR("\x1b[0m" << __FILE__ << ":" << __LINE__ << ": WARNING: " << message); \
	} while (0)

#define UNIMPLEMENTED(message) \
	do { \
		LOG_ERROR("\x1b[0m" << __FILE__ << ":" << __LINE__ << ": UNIMPLEMENTED: " << message); \
		exit(1); \
	} while (0)

#define UNREACHABLE(message) \
	do { \
		LOG_ERROR("\x1b[0m" << __FILE__ << ":" << __LINE__ << ": UNREACHABLE: " << message); \
		exit(1); \
	} while (0)

#define RED 	"\x1B[31m"
#define GREEN 	"\x1B[32m"
#define YELLOW 	"\x1B[33m"
#define END		"\033[0m\t\t"

enum class State {
	ParsingHeaders,
	ParsingBody,
	BuildingResponse,
	ProcessingCGI,
	UploadingFile,
	FetchingFile,
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

