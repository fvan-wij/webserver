#include "Server.hpp"
#include "HttpObject.hpp"
#include <cstring>


int main()
{
	std::vector<Server> servers;

	// parse config
	// for each server_config in config.serverblock
	// servers.push_back(server(serverrconfig))

	servers.push_back({{8080, 8081}});
	servers.push_back({{9090, 9091}});

	while (1)
	{
		for(auto &s : servers)
		{
			if (s.poll() > 0)
				s.handle_events();
		}
	}
	return 0;
}

// int main () {
// 	std::string response = {
// 		":authority: stackoverflow.com\r\n"
// 			":method: GET\r\n"
// 			":path: /questions/14070940/how-can-i-print-out-c-map-values\r\n"
// 			":scheme: https\r\n"
// 			"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\n"
// 			"Accept-Encoding: gzip, deflate, br\r\n"
// 			"Accept-Language: en-GB,en;q=0.9,en-US;q=0.8,nl;q=0.7\r\n"
// 			"Cache-Control: max-age=0\r\n"
// 			"Referer: https://duckduckgo.com/\r\n"
// 			"Sec-Ch-Ua-Mobile: ?0\r\n"
// 			"User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36\r\n"
// 			"Content-Length: 27\r\n"
// 			"\r\n<h1> Fakka strijders </h1>\r\n"
// 	};
//
// 	std::string post = 
// 	{
// 		"POST /posts HTTP/1.1\r\n"
// 			"Host: example.com\r\n"
// 			"Accept: application/json\r\n"
// 			"Authorization: Basic dXNlcjpwYXNz\r\n"
// 			"Content-Type: application/json\r\n"
// 			"Content-Length: 42\r\n"
//
// 			"\r\n<h1> Fakka strijders </h1>\r\n"
// 			"\r\n"
// 	};
//
//
// 	HttpObject responseObject = HttpObject(response);
// 	HttpObject postObject = HttpObject(post);
//
// 	std::cout << responseObject << std::endl;
// 	std::cout << responseObject.get_value("Content-Length") << std::endl;
//
// 	std::cout << postObject << std::endl;
// 	std::cout << postObject.get_value("Content-Length") << std::endl;
// }
