#pragma once
#ifndef TESTLOGGER_HPP
#define TESTLOGGER_HPP

#include <string>
#include <sstream>

#define TEST_LOG(Logger_, Message_)                  \
  Logger_(                                      \
    static_cast<std::ostringstream&>(           \
      std::ostringstream().flush() << Message_  \
    ).str(),                                    \
    __FUNCTION__,                               \
    __FILE__,                                   \
    __LINE__                                    \
  )

#define TEST_PASSED(Message_) TEST_LOG(TestLogger::Passed(), Message_)
#define TEST_FAILED(Message_) TEST_LOG(TestLogger::Failed(), Message_)


extern std::stringstream globalErrorStream;

class TestLogger
{
public:

	enum class Level : int
	{
		FAILED,
		PASSED
	};

	TestLogger(Level l);
	TestLogger(const TestLogger &) = delete;
	TestLogger &operator=(const TestLogger &) = delete;
	~TestLogger();

	void operator()(std::string const& message,
					char const* function,
					char const* file,
					int line);

	static TestLogger &Failed();
	static TestLogger &Passed();

private:
	TestLogger();

	Level _level;
	void log(const std::string &s);
};

#endif // TESTLOGGER_HPP