#include "GetRequestHandler.hpp"
#include "Logger.hpp"
#include <fstream>

std::string GetRequestHandler::retrieve_html(std::string_view path)
{
	constexpr auto 	read_size = std::size_t(1024);
	auto 			file_stream = std::ifstream(path.data(), std::ios::binary);
	//Read page, dump in body string;
	if (not file_stream)
		LOG_ERROR("Could not open html file");
	else
	{
		auto out = std::string();
		auto buf = std::string(read_size, '\0');
		while (file_stream.read(&buf[0], read_size))
		{
			out.append(buf, 0, file_stream.gcount());
		}
		out.append(buf, 0, file_stream.gcount());
		return out;
	}
	std::string out(0);
	return out;
}

HttpResponse	GetRequestHandler::handle_request(const HttpRequest &request)
{
	HttpResponse response;
	LOG_NOTICE("Handling GET request...\n" << request);
	response.set_status_code(200);
	response.set_status_mssg("OK");
	// if (request.get_uri() == "/" || request.get_uri() == "/favicon.ico")
	if (request.get_uri() == "/")
	{
		response.set_body("\r\n" + retrieve_html("./var/www/html/index.html") + "\r\n");
	}
	else {
		response.set_body("\r\n<h1>Fakka strijders</h1>\n<form action='' method='post'><button name='foo' value='Yeet'>Yeet</button></form>\r\n");
	}
	response.set_state(READY);
	response.set_type(ResponseType::REGULAR);
	return response;
}
