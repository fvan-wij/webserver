#include "PostRequestHandler.hpp"

HttpResponse	PostRequestHandler::handle_request(const HttpRequest &request, t_config &config)
{
	(void) config;
	HttpResponse response;
	//Should trigger CGI
	// LOG_NOTICE("Handling POST request...\n" << request);
	LOG_NOTICE("Handling POST request...\n");
	response.set_status_code(200);
	response.set_status_mssg("OK");
	if (content_length_exceeded(request, config))
	{
		std::string mssg = "<h1>400 Bad Request - client_max_body_size exceeded</h1>";
		response.set_body("\r\n" + mssg + "\r\n");
		response.set_state(READY);
		response.set_type(ResponseType::ERROR);
		LOG_ERROR("client max body exceeded");
	}
	else if (request.get_uri() == "/cgi-bin")
	{
		std::string mssg = "<h1>CGI data</h1>";
		response.set_body("\r\n" + mssg + "\r\n");
		response.set_state(NOT_READY);
		response.set_type(ResponseType::CGI);
	}
	else
	{
		std::vector<char> buffer = request.get_body();
		std::string_view sv_buffer(buffer.data(), buffer.size());
		size_t pos = sv_buffer.find("filename=");
		if (pos != std::string::npos)
		{
			size_t end = sv_buffer.find(10, pos);
			pos+=10;
			std::string filename(&sv_buffer[pos], (end - 2) - (pos));
			filename.push_back('\0');
			std::ofstream outfile("." + request.get_uri() + "/" + filename.data(), std::ios::binary);
			outfile.write(buffer.data(), buffer.size());
			outfile.close();
		}
		std::string mssg = "<h1>File uploaded successfully</h1>";
		response.set_body("\r\n" + mssg + "\r\n");
		response.set_state(READY);
		response.set_type(ResponseType::REGULAR);
	}
	return response;
}
