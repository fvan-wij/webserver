#pragma once

#include <string>
#include <string_view>
#include <vector>

class CgiHandler
{
	public:
		CgiHandler(std::string path, std::string url_params, std::string& body_buff, char **envp);
		CgiHandler(const CgiHandler &) = delete;
		CgiHandler &operator=(const CgiHandler &) = delete;
		~CgiHandler();

		void handle_cgi(short revents);
		void verify(std::string_view uri, std::string &param, std::string &body, char *const envp[]);

		void start(char *const envp[]);
		bool poll();
		void kill();

		enum PipeFD
		{
			READ	= 0,
			WRITE	= 1,
			COUNT	= 2,
		};


		const std::string 	&get_buffer() const;
		const int			&get_pipe_fd() const;
		bool				is_running() {return _is_running;};
		bool				is_finished() {return _is_finished;};

	private:
		bool		_is_running;
		bool		_is_finished;
		bool 		_is_killed;
		bool 		_error;
		std::string _buffer;
		int			_pid;
		int			_pipes[PipeFD::COUNT];
		std::vector<std::string> _argv;


		int32_t _read_pipe();
};

