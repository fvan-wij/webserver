// HttpRequest.h
#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <optional>

class HttpRequest {
public:
    // Supported HTTP methods
    enum class Method {
        GET, POST, PUT, DELETE, UNKNOWN
    };

    // Constructor (initially empty)
    HttpRequest();

    // Parse the raw HTTP request data
    bool parse(const std::vector<char>& raw_request);

    // Accessors
    Method getMethod() const;
    const std::string& getPath() const;
    const std::string& getVersion() const;
    std::optional<std::string> getHeader(const std::string& name) const;
    const std::string& getBody() const;

private:
    // Parsed data
    Method method_;
    std::string path_;
    std::string version_;
    std::unordered_map<std::string, std::string> headers_;
    std::string body_;

    // Helper methods
    Method parseMethod(const std::string& method_str);
    bool parseRequestLine(const std::string& line);
    bool parseHeaders(const std::string& headers_section);
};