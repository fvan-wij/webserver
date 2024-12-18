#include "TestLogger.hpp"
#include "meta.hpp"
#include "ansi.h"
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

std::stringstream globalErrorStream;

TestLogger::TestLogger(TestLogger::Level level) : _level(level)
{

}

TestLogger &TestLogger::Failed()
{
	static TestLogger l(TestLogger::Level::FAILED);
	return l;
}

TestLogger &TestLogger::Passed()
{
	static TestLogger l(TestLogger::Level::PASSED);
	return l;
}

void TestLogger::operator()(std::string const& message, char const* function, char const* file, int line)
{
	UNUSED(function);
	UNUSED(file);
	UNUSED(line);

	const char *PREFIX[] = {
		"FAILED",
		"PASSED",
	};

	const char *COLORS[] = {
		COLOR_RED,
		COLOR_GREEN,
	};

    const time_t t = time(0);
    const struct tm ct = *localtime(&t);

	std::stringstream time;
	time	<< "[" << std::setfill('0') << std::setw(2) << ct.tm_hour
		<< ":" << std::setfill('0') << std::setw(2) << ct.tm_min
		<< ":" << std::setfill('0') << std::setw(2) << ct.tm_sec << "]";
	std::cerr << time.str() << "[" << COLORS[int(_level)] << PREFIX[int(_level)]<< COLOR_RESET << "]";
	if (_level == TestLogger::Level::FAILED)
	{
		std::cerr << " : "<< message << std::endl;
		std::cerr << globalErrorStream.str() << std::endl;
		globalErrorStream.str("");
	}
	else
		std::cerr << " : "<< message << std::endl;


}

TestLogger::TestLogger()
{

}

TestLogger::~TestLogger()
{

}