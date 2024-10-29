#include "ConnectionManager.hpp"
#include "ConnectionInfo.hpp"
#include "Logger.hpp"
#include "ConfigParser.hpp"

#include <cstring>
#include <optional>
#include <sys/poll.h>
#include <vector>

// #define USE_TEST_MAIN

#ifndef USE_TEST_MAIN

void loop(ConnectionManager &cm)
{
	while (1)
	{
		std::vector<pollfd>& pfds = cm.get_pfds();
		std::vector<FdType>& fd_types = cm.get_fd_types();
		int n_ready = ::poll(pfds.data(), pfds.size(), POLL_TIMEOUT);
		if (n_ready > 0)
		{
			for (size_t i = 0; i < pfds.size(); i++)
			{
				pollfd &pfd = pfds[i];
				FdType &type = fd_types[i];
				ConnectionInfo &ci = *cm.get_connection_info()[pfd.fd].get();
				if (type == FdType::LISTENER && pfd.revents & POLLIN)
				{
					LOG_INFO("fd: " << pfd.fd << " POLLIN (listener)");
					cm.add_client(ci);
				}
				else if (type == FdType::CLIENT && pfd.revents & POLLIN)
				{
					// Parse request and generate response
					LOG_INFO("fd: " << pfd.fd << " POLLIN (client)");
					std::optional<std::vector<char>> read_data = ci.get_socket().read();
					auto protocol = ci.get_protocol();
					if (read_data)
					{
						std::vector<char> data = read_data.value();
						protocol->handle(data);
						if (protocol->response.get_type() == ResponseType::CGI && !protocol->is_cgi_running())
						{
							protocol->start_cgi();
							LOG_INFO("Starting CGI on port: " << ci.get_socket().get_port());
							cm.add_pipe(pfd.fd, protocol->get_pipe_fd());
						}
					}
					else
					{
						cm.remove(i);
					}
				}
				else if (type == FdType::PIPE && pfd.revents & POLLIN)
				{
					// Read from pipe
					LOG_INFO("fd: " << pfd.fd << " POLLIN (pipe)");
					auto protocol = ci.get_protocol();
					protocol->poll_cgi();
				}
				else if (type == FdType::CLIENT && pfd.revents & POLLOUT)
				{
					// Send response
					// LOG_INFO("fd: " << pfd.fd << " POLLOUT (client)");
					auto protocol = ci.get_protocol();
					if (protocol->response.is_ready())
					{
						std::string data = protocol->get_data();
						LOG_INFO("Sending response..." << protocol->response.get_status_code() << " " << protocol->response.get_status_mssg());
						ci.get_socket().write(data);
						if (protocol->response.get_type() == ResponseType::CGI) // Remove pipe_fd && pipe type
						{
							cm.remove_pipe(pfd.fd);
						}
						cm.remove(i);
					}
					else if (protocol->response.get_type() == ResponseType::UPLOAD)
					{
						protocol->poll_upload();
					}
					else if (protocol->response.get_type() == ResponseType::FETCH_FILE) // Note: Fetching requires both reading and writing... but does both when there's a POLLOUT revent.
					{
						// LOG_INFO("fd: " << pfd.fd << " POLLOUT (Fetch file)"); 
						protocol->poll_fetch();
					}
				}
				else if (pfd.revents & POLLERR)
				{
					LOG_ERROR("POLLERR error occurred with fd: " << pfd.fd << ", type: " << int(type));
				}
				else if (pfd.revents & POLLNVAL)
				{
					LOG_ERROR("POLLNVAL error occurred with fd: " << pfd.fd << ", type: " << int(type));
				}
			}
		}
	}
}

int main(int argc, char *argv[])
{
	std::vector<t_config>	configs;
	ConnectionManager		cm;

	if (argc == 2 && argv[1]) // Do check if file extension is correct (.conf)
	{
		configs = parse_config(argv[1]);
	}
	else
	{
		LOG_ERROR("Config is invalid or not present, using DEFAULT_CONFIG");
		configs.push_back(DEFAULT_CONFIG);
		// return -1;
	}
	cm.add_listeners(configs);
	loop(cm);
}

#endif
