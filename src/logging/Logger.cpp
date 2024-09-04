#include "Logger.hpp"
#include "meta.hpp"
#include "ansi.hpp"
#include <iostream>
#include <string>



Logger::Logger(Logger::Level level) : _level(level)
{

}

Logger &Logger::Error()
{
	static Logger l(Logger::Level::ERROR);
	return l;
}

Logger &Logger::Warning()
{
	static Logger l(Logger::Level::WARNING);
	return l;
}

Logger &Logger::Info()
{
	static Logger l(Logger::Level::INFO);
	return l;
}

Logger &Logger::Debug()
{
	static Logger l(Logger::Level::DEBUG);
	return l;
}




void Logger::operator()(std::string const& message, char const* function, char const* file, int line)
{
	UNUSED(function);

	const char *PREFIX[] = {
		COLOR_RED		"[ERROR]   : ",
		COLOR_YELLOW	"[WARNING] : ",
		COLOR_CYAN		"[INFO]    : ",
		COLOR_WHITE		"[DEBUG]   : ",
	};

	std::cout << PREFIX[int(_level)] << message;
	if (_level == Level::DEBUG)
		std::cout << "\t @ " << file << ":" << line;

	std::cout << COLOR_RESET << std::endl;
}


Logger::Logger()
{

}

Logger::~Logger()
{

}
