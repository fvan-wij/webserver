#pragma once

#include <ConnectionManager.hpp>

class CGIHandler
{
	public:
		CGIHandler(ConnectionManager &cm);
		~CGIHandler();

		enum PipeFD
		{
			READ	= 0,
			WRITE	= 1,
			COUNT	= 2,
		};

		void	handle_cgi(short events);
		void	execute_cgi(std::vector<const char*> args, char *const envp[]);
		void	read_pipe();


	private:
		ConnectionManager	&_cm;
		int 				_pipes[2];

		pid_t				_pid;
		bool				_is_running;
		std::vector<const char*> args;
		char *				envp[];

		void	child_process(std::vector<const char*> args, char *const envp[])

};