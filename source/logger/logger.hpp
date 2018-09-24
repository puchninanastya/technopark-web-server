#ifndef MONZZA_LOGGER_HPP
#define MONZZA_LOGGER_HPP

#include <thread>
#include <functional>

#include "../../external/cpl/cpl_event_queue.hpp"

#include "log_level.hpp"
#include "log_message.hpp"
#include "logger_thread.hpp"

class Logger {
public:
	bool start();
	cpl::Event* getNewLogMessageEvent();
	void addLogMessage( LogMessage* logMessage );
	LogMessage* getLogMessage();
	bool stop();
private:
	std::thread* thread_;
	LoggerThread* loggerThread_;
};

#endif // MONZZA_LOGGER_HPP