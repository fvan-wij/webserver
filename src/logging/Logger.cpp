#include "Logger.hpp"
#include "meta.hpp"
#include "ansi.hpp"
#include <chrono>
#include <ctime>
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

	const char *COLORS[] = {
		COLOR_RED,
		COLOR_YELLOW,
		COLOR_CYAN,
		COLOR_WHITE,
	};


    time_t t = time(0);  
    struct tm ct = *localtime(&t);


	std::cout << COLORS[int(_level)] << "[" << ct.tm_hour << ":" << ct.tm_min << ":" << ct.tm_sec << "]" << PREFIX[int(_level)] << message;
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
