#include <exception>
#include <string>

class HttpException : public std::exception 
{
	public:

		HttpException(int status, const std::string& message) : _status_code(status), _message(message){};

		int				status()	const noexcept {return _status_code;};
		const char* 	what() 		const noexcept override {return _message.c_str();};

	private:
		int			_status_code;
		std::string _message;
};

class HttpRedirection: public std::exception 
{
	public:

		HttpRedirection(int status, const std::string& redirection) : _status_code(status), _redirection(redirection){};

		int				status()	const noexcept {return _status_code;};
		const char* 	what() 		const noexcept override {return _redirection.c_str();};

	private:
		int			_status_code;
		std::string _redirection;
};

class ClosedConnectionException: public std::exception 
{
	public:

		ClosedConnectionException(const std::string& message) : _message(message){};

		const char* 	what() 		const noexcept override {return _message.c_str();};

	private:
		std::string _message;
};
