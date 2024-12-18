#include <iostream>
#include <vector>
#include "HttpRequest.hpp"
#include <cstring>
#include <TestLogger.hpp>
#include <Logger.hpp>

// Use the helper function to convert a string to a vector<char>
std::vector<char> stringToVector(const char* str) {
	return std::vector<char>(str, str + std::strlen(str));
}

// std::string globalErrorMessage;

// Wrapper for raw request data logging
void logRawRequest(const std::vector<char>& raw_request) {
	std::stringstream ss;
	ss << "Request Data: \n";
	for (char c : raw_request) {
		ss << c;
	}
	ss << "\n";
	globalErrorStream << ss.str();
}


bool testValidGetRequest(bool log = false) {
	std::vector<char> raw_request = stringToVector(
		"GET /index.html HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"User-Agent: TestClient\r\n"
		"\r\n"
	);

	HttpRequest request;
	if (request.parse(raw_request)) {
		if (log) {
			std::cout << "Valid GET Request Parsed Successfully!\n";
			std::cout << "Method: " << (request.getMethod() == HttpRequest::Method::GET ? "GET" : "Unknown") << "\n";
			std::cout << "Path: " << request.getPath() << "\n";
			std::cout << "Version: " << request.getVersion() << "\n";
			std::cout << "Host Header: " << request.getHeader("Host").value_or("Not Found") << "\n";
			std::cout << "User-Agent Header: " << request.getHeader("User-Agent").value_or("Not Found") << "\n";
		}
		if (request.getMethod() != HttpRequest::Method::GET) {
			globalErrorStream << "Failed to parse GET method.\n";
			return false;
		}
		return true;
	} else {
		globalErrorStream << "Failed to parse valid GET request.\n";
		logRawRequest(raw_request);
		return false;
	}
}

bool testValidPostRequest(bool log = false) {
	std::vector<char> raw_request = stringToVector(
		"POST /submit HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"Content-Length: 13\r\n"
		"\r\n"
		"key=value&key2=value2"
	);

	HttpRequest request;
	if (request.parse(raw_request)) {
		if (log) {
			std::cout << "Valid POST Request Parsed Successfully!\n";
			std::cout << "Method: " << (request.getMethod() == HttpRequest::Method::POST ? "POST" : "Unknown") << "\n";
			std::cout << "Path: " << request.getPath() << "\n";
			std::cout << "Version: " << request.getVersion() << "\n";
			std::cout << "Host Header: " << request.getHeader("Host").value_or("Not Found") << "\n";
			std::cout << "Body: " << request.getBody() << "\n";
		}
		if (request.getMethod() != HttpRequest::Method::POST) {
			globalErrorStream << "Failed to parse POST method.\n";
			return false;
		}
		return true;
	} else {
		globalErrorStream << "Failed to parse valid POST request.\n";
		logRawRequest(raw_request);
		return false;
	}
}

// Invalid request line (missing method)
bool testInvalidRequestLine(bool log = false) {
	std::vector<char> raw_request = stringToVector(
		"INVALID /path HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"\r\n"
	);

	HttpRequest request;
	if (request.parse(raw_request))
	{
		if (request.getMethod() == HttpRequest::Method::UNKNOWN) {
			if (log) {
				std::cout << "Invalid Request Line Handled Correctly.\n";
			}
			return true;
		} else {
			globalErrorStream << "Failed to detect invalid request line.\n";
			logRawRequest(raw_request);
			return false;
		}
	}
	else
	{
		globalErrorStream << "Failed to parse invalid request line.\n";
		logRawRequest(raw_request);
		return false;
	}
}

// Missing colon in Host header
bool testInvalidHeaders(bool log = false) {
	std::vector<char> raw_request = stringToVector(
		"GET /index.html HTTP/1.1\r\n"
		"Host example.com\r\n"
		"\r\n"
	);

	HttpRequest request;
	if (!request.parse(raw_request)) {
		if (log) {
			std::cout << "Invalid Headers Handled Correctly.\n";
		}
		return true;
	} else {
		globalErrorStream << "Failed to detect invalid headers.\n";
		logRawRequest(raw_request);
		return false;
	}
}

void runTest(const std::string& testName, bool (*testFunc)(bool), bool log = false) {
	if (testFunc(log)) {
		TEST_PASSED(testName);
	} else {
		TEST_FAILED(testName);
	}
}

int HttpRequestTest(bool log = false) {
	LOG_NOTICE("--- Testing HttpRequest Class ---");

	runTest("Valid GET Request", testValidGetRequest, log);
	runTest("Valid POST Request", testValidPostRequest, log);
	runTest("Invalid Request Line", testInvalidRequestLine, log);
	runTest("Invalid Headers", testInvalidHeaders, log);

	return 0;
}
