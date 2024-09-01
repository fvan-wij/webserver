#pragma once

#define POLL_TIMEOUT 100
#define LISTEN_BACKLOG 8

#define LOG_ERROR(x) std::cerr << "[ERROR] : " << x << std::endl
#define LOG(x) std::cout << "[INFO] : " << x << std::endl

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
