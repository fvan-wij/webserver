#include "RequestHandler.hpp"

RequestHandler::~RequestHandler()
{
	// LOG(RED << "Deleting requesthandler" << END);
}

std::string RequestHandler::retrieve_html(std::string_view path)
{
	constexpr auto 	read_size 	= std::size_t(1024);
	auto 			file_stream = std::ifstream(path.data(), std::ios::binary);
	auto 			out 		= std::string();

	if (not file_stream)
		LOG_ERROR("Could not open file");
	else
	{
		auto buf = std::string(read_size, '\0');
		while (file_stream.read(&buf[0], read_size))
		{
			out.append(buf, 0, file_stream.gcount());
		}
		out.append(buf, 0, file_stream.gcount());
		return out;
	}
	return out;
}
