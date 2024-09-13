#pragma once

#include <iostream>
#include "Logger.hpp"


#define POLL_TIMEOUT 100
#define LISTEN_BACKLOG 8

#define SOCKET_READ_SIZE 4096

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
