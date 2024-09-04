// Based on this very sick post!??!
// https://stackoverflow.com/a/8338089/7363348

#pragma once

#include <string>
#include <sstream>


#define LOG(Logger_, Message_)                  \
  Logger_(                                      \
    static_cast<std::ostringstream&>(           \
      std::ostringstream().flush() << Message_  \
    ).str(),                                    \
    __FUNCTION__,                               \
    __FILE__,                                   \
    __LINE__                                    \
  )


#define LOG_ERROR(Message_) LOG(Logger::Error(), Message_)
#define LOG_WARNING(Message_) LOG(Logger::Warning(), Message_)
#define LOG_INFO(Message_) LOG(Logger::Info(), Message_)
#define LOG_NOTICE(Message_) LOG(Logger::Notice(), Message_)
#define LOG_DEBUG(Message_) LOG(Logger::Debug(), Message_)



class Logger
{
public:

	enum class Level : int
	{
		ERROR,
		WARNING,
		INFO,
		NOTICE,
		DEBUG,
	};

	Logger(Level l);
	Logger(const Logger &) = delete;
	Logger &operator=(const Logger &) = delete;
	~Logger();

	void operator()(std::string const& message,
					char const* function,
					char const* file,
               	    int line);




	static Logger &Error();
	static Logger &Warning();
	static Logger &Info();
	static Logger &Notice();
	static Logger &Debug();

private:
	Logger();

	Level _level;

	void log(const std::string &s);
	
};

