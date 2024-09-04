#include "Logger.hpp"
#include "meta.hpp"
#include "ansi.hpp"
#include <ctime>
#include <iomanip>
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
		COLOR_RED		"[ERROR] : ",
		COLOR_YELLOW	"[WARNING] : ",
		COLOR_CYAN		"[INFO] : ",
		COLOR_WHITE		"[DEBUG] : ",
	};

	const char *COLORS[] = {
		COLOR_RED,
		COLOR_YELLOW,
		COLOR_CYAN,
		COLOR_WHITE,
	};

    const time_t t = time(0);  
    const struct tm ct = *localtime(&t);



	std::stringstream time;
	time	<< "[" << std::setfill('0') << std::setw(2) << ct.tm_hour
			<< ":" << std::setfill('0') << std::setw(2) << ct.tm_min
			<< ":" << std::setfill('0') << std::setw(2) << ct.tm_sec << "]";

	std::cout << COLORS[int(_level)] << time.str() << PREFIX[int(_level)] << message;
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
