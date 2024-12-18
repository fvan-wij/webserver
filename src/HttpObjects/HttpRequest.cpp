// HttpRequest.cpp
#include <HttpRequest.hpp>
#include <sstream>
#include <algorithm>
#include <cctype>

/**
 * @brief Default constructor for HttpRequest.
 * Initializes an empty HttpRequest object with no method, path, version, headers, or body.
 */
HttpRequest::HttpRequest() : method_(Method::UNKNOWN), path_(), version_(), headers_(), body_() {}

/**
 * @brief Parses the raw HTTP request data.
 *
 * Converts the raw HTTP request data from a vector of characters into structured fields,
 * including the method, path, version, headers, and body.
 *
 * @param raw_request The raw HTTP request data as a vector of characters.
 * @return True if parsing succeeds; false otherwise.
 */
bool HttpRequest::parse(const std::vector<char>& raw_request) {
    std::string request(raw_request.begin(), raw_request.end());
    std::istringstream stream(request);
    std::string line;

    if (!std::getline(stream, line) || !parseRequestLine(line)) {
        return false;
    }

    std::string headers_section;
    while (std::getline(stream, line) && line != "\r") {
        headers_section += line + "\n";
    }
    if (!parseHeaders(headers_section)) {
        return false;
    }

    body_.assign(std::istreambuf_iterator<char>(stream), {});
    return true;
}

/**
 * @brief Retrieves the HTTP method of the request.
 *
 * @return The HTTP method as an enum value of HttpRequest::Method.
 */
HttpRequest::Method HttpRequest::getMethod() const {
    return method_;
}

/**
 * @brief Retrieves the requested path from the HTTP request.
 *
 * @return A constant reference to the request path as a string.
 */
const std::string& HttpRequest::getPath() const {
    return path_;
}

/**
 * @brief Retrieves the HTTP version of the request.
 *
 * @return A constant reference to the HTTP version as a string.
 */
const std::string& HttpRequest::getVersion() const {
    return version_;
}

/**
 * @brief Retrieves the value of a specific header, if it exists.
 *
 * @param name The name of the header to retrieve.
 * @return An optional string containing the header value, or std::nullopt if the header does not exist.
 */
std::optional<std::string> HttpRequest::getHeader(const std::string& name) const {
    auto it = headers_.find(name);
    if (it != headers_.end()) {
        return it->second;
    }
    return std::nullopt;
}

/**
 * @brief Retrieves the body content of the HTTP request.
 *
 * @return A constant reference to the body content as a string.
 */
const std::string& HttpRequest::getBody() const {
    return body_;
}

/**
 * @brief Converts the HTTP method string to the corresponding enum value.
 *
 * Matches the provided method string to an HttpRequest::Method enum value.
 * Returns HttpRequest::Method::UNKNOWN for unrecognized methods.
 *
 * @param method_str The HTTP method as a string (e.g., "GET", "POST").
 * @return The corresponding HttpRequest::Method enum value.
 */
HttpRequest::Method HttpRequest::parseMethod(const std::string& method_str) {
    if (method_str == "GET") return Method::GET;
    if (method_str == "POST") return Method::POST;
    if (method_str == "PUT") return Method::PUT;
    if (method_str == "DELETE") return Method::DELETE;
    return Method::UNKNOWN;
}

/**
 * @brief Parses the request line of the HTTP request.
 *
 * Extracts and sets the HTTP method, path, and version from the request line.
 *
 * @param line The request line as a string (e.g., "GET /index.html HTTP/1.1").
 * @return True if the request line is valid; false otherwise.
 */
bool HttpRequest::parseRequestLine(const std::string& line) {
    std::istringstream line_stream(line);
    std::string method_str, version;

    if (!(line_stream >> method_str >> path_ >> version)) {
        return false;
    }

    method_ = parseMethod(method_str);
    version_ = version;
    return true;
}

/**
 * @brief Parses the headers section of the HTTP request.
 *
 * Extracts and stores headers as key-value pairs in a map.
 *
 * @param headers_section The headers section as a string.
 * @return True if headers are successfully parsed; false otherwise.
 */
bool HttpRequest::parseHeaders(const std::string& headers_section) {
    std::istringstream headers_stream(headers_section);
    std::string line;

    while (std::getline(headers_stream, line)) {
        auto colon_pos = line.find(':');
        if (colon_pos == std::string::npos) {
            return false;
        }

        std::string name = line.substr(0, colon_pos);
        std::string value = line.substr(colon_pos + 1);

        name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);

        headers_[name] = value;
    }
    return true;
}
