#include "Logger.hpp"
#include "Server.hpp"
#include "HttpRequest.hpp"
#include "HandlerFactory.hpp"
#include "ConfigParser.hpp"

#include <cstring>

// #define USE_TEST_MAIN

#ifndef USE_TEST_MAIN
int main(int argc, char *argv[])
{
	std::vector<t_config> 	configs;
	std::vector<Server> 	servers;

	// parse config
	if (argc > 1)
		configs = parse_config(argv[1]);

	if (!configs.empty())
	{
		for (auto &it : configs)
			servers.push_back(it);
	}
	else
	{
		LOG_ERROR("Could not create server(s) from given config file. Did you supply a config file?");
		return 1;
	}
	LOG_NOTICE("Starting server(s)");
	for(const Server &s : servers)
	{
		LOG_NOTICE(s);
	}

	while (1)
	{
		for(auto &s : servers)
		{
			if (s.should_exit())
				return 0;
			else if (s.poll() > 0)
				s.handle_events();
		}
	}
	return 0;
}
#endif

#ifdef USE_TEST_MAIN
const std::string response = {
	":authority: stackoverflow.com\r\n"
		":method: GET\r\n"
		":path: /questions/14070940/how-can-i-print-out-c-map-values\r\n"
		":scheme: https\r\n"
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\n"
		"Accept-Encoding: gzip, deflate, br\r\n"
		"Accept-Language: en-GB,en;q=0.9,en-US;q=0.8,nl;q=0.7\r\n"
		"Cache-Control: max-age=0\r\n"
		"Referer: https://duckduckgo.com/\r\n"
		"Sec-Ch-Ua-Mobile: ?0\r\n"
		"User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36\r\n"
		"Content-Length: 27\r\n"
		"\r\n<h1> Fakka strijders </h1>\r\n"
};

const std::string post = 
{
	"POST /cgi-bin HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"Accept: application/json\r\n"
		"Authorization: Basic dXNlcjpwYXNz\r\n"
		"Content-Type: application/json\r\n"
		"Content-Length: 42\r\n"
		"\r\n"
		"foo=Yeet"
};

const std::string invalid_method = 
{
	"KAAS /posts HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"Accept: application/json\r\n"
		"Authorization: Basic dXNlcjpwYXNz\r\n"
		"Content-Type: application/json\r\n"
		"Content-Length: 42\r\n"
		"\r\n<h1> Fakka strijders </h1>\r\n"
		"\r\n"
};

const std::string get = 
{
	"GET / HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"Accept: application/json\r\n"
		"Authorization: Basic dXNlcjpwYXNz\r\n"
		"Content-Type: application/json\r\n"
		"Content-Length: 42\r\n"
		"\r\n"
};

const std::string get_real = 
    "GET / HTTP/1.1\r\n"
    "Host: localhost:9090\r\n"
    "Connection: keep-alive\r\n"
    "Cache-Control: max-age=0\r\n"
    "sec-ch-ua: \"Not_A Brand\";v=\"8\", \"Chromium\";v=\"120\", \"Google Chrome\";v=\"120\"\r\n"
    "sec-ch-ua-mobile: ?0\r\n"
    "sec-ch-ua-platform: \"Linux\"\r\n"
    "Upgrade-Insecure-Requests: 1\r\n"
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36\r\n"
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\n"
    "Sec-Fetch-Site: none\r\n"
    "Sec-Fetch-Mode: navigate\r\n"
    "Sec-Fetch-User: ?1\r\n"
    "Sec-Fetch-Dest: document\r\n"
    "Accept-Encoding: gzip, deflate, br\r\n"
    "Accept-Language: en-GB,en;q=0.9,en-US;q=0.8,nl;q=0.7\r\n\r\n";

const std::string dummy_upload =
    "POST /upload HTTP/1.1\r\n"
    "Host: example.com\r\n"
    "Content-Type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW\r\n"
    "Content-Length: 1024\r\n"
    "\r\n"
    "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\n"
    "Content-Disposition: form-data; name=\"file\"; filename=\"dummy_image.png\"\r\n"
    "Content-Type: image/png\r\n"
    "\r\n"
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    "\r\n"
    "------WebKitFormBoundary7MA4YWxkTrZu0gW--\r\n";

const std::string img = {
"POST /upload HTTP/1.1\r\n"
"Host: example.com\r\n"
"Content-Type: multipart/form-data; boundary=---------------------------123456789\r\n"
"Content-Length: [LENGTH_OF_CONTENT]\r\n"
"\r\n"
"-----------------------------123456789\r\n"
"Content-Disposition: form-data; name='file'; filename='small_image.png'\r\n"
"Content-Type: image/png\r\n"
"\r\n"
"iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR42mP8/wcAAwAB/DFZZQAAAABJRU5ErkJggg==\r\n"
"-----------------------------123456789--\r\n"
};

#include "Config.hpp"


// http_server-> state machine for handling connections
// states: 
// - reading headers
// - reading body
// - generating response
//	state reading headers:
//		Reading and parsing header, once done, store parsed data in request
// 		Reading and parsing body, once done, store parsed data in request
// 		Pass request to requesthandler, generates response
//		Once done, send response to client and drop connection

// void	test_from_file(HttpServer &http_server)
// {
// 	std::ifstream fin("./dummy_http.test", std::ifstream::binary);
// 	std::vector<char> buffer (512, 0);
// 	if (!fin.is_open())
// 		LOG_ERROR("FILE NOT OPEN!");
// 	while (1)
// 	{
// 		fin.read(buffer.data(), buffer.size());
// 		std::streamsize size = fin.gcount();
// 		// LOG_DEBUG(buffer.data());
// 		http_server.handle({buffer.begin(), buffer.begin() + size});
// 	}
// }

void	test_from_string(HttpServer &http_server, std::string test)
{
	size_t i = 0;
	size_t chunk_size = 16;
	while (!http_server.response.is_ready())
	{
		size_t remaining = test.size() - i;
		size_t current_chunk = std::min(chunk_size, remaining);
		std::vector<char> char_vec(test.begin() + i, test.begin() + i + current_chunk);
		http_server.handle(char_vec);
		i += current_chunk;
	}
	LOG_INFO("==SENDING RESPONSE==\n" << http_server.response.to_string());
}

int main () {
	HttpServer http_server = HttpServer();

	test_from_string(http_server, dummy_upload);

	// auto handler = HandlerFactory::create_handler(request.get_type());
	// HttpResponse response = handler->handle_request(request, TEST_CONFIG);
	// std::cout << response.to_string() << std::endl;
	return 0;
}
#endif

