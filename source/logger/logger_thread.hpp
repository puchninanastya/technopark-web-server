#ifndef MONZZA_LOGGER_THREAD_HPP
#define MONZZA_LOGGER_THREAD_HPP

#include "../../external/spdlog/spdlog.h"
#include "../../external/cpl/cpl_event_queue.hpp"

#include "log_level.hpp"
#include "log_message.hpp"

namespace monzza {
    namespace logger {
        class LoggerThreadServiceMessage {
        public:
            enum class ResponseType {
                NOT,
                SUCCESSFUL,
                FAILURE,
            };

            enum class CommandType {
                STOP
            };

            bool setResponseType( ResponseType responseType );
            ResponseType getResponseType() const;

            bool setCommandType( CommandType commandType );
            CommandType getCommandType() const;
        private:
            ResponseType responseType_;
            CommandType  commandType_;
        };

        class LoggerThread {
        public:
            LoggerThread();

            cpl::Event* getNewOutputServiceMessageEvent();
            void addInputServiceMessage( LoggerThreadServiceMessage* loggerThreadServiceMessage );
            LoggerThreadServiceMessage* getOutputServiceMessage();

            cpl::Event* getNewLogMessageEvent();
            void addLogMessage( LogMessage* logMessage );
            LogMessage* getLogMessage();

            void operator()();
        private:
            void processInputServiceMessage();
            void processLogMessages();

            void serviceMsgHandler( LoggerThreadServiceMessage* loggerThreadServiceMessage );
            void stopServiceMsgHandler( LoggerThreadServiceMessage* loggerThreadServiceMessage );

            bool breakThreadLoop_;

            std::shared_ptr<spdlog::logger> spdLogger_;

            std::vector<cpl::Event*> events_;

            cpl::EventQueue<LoggerThreadServiceMessage*> inputServiceMessages_;
            cpl::EventQueue<LoggerThreadServiceMessage*> outputServiceMessages_;

            LogLevel logLevel_;
            cpl::EventQueue<LogMessage*> logMessages_;
            cpl::EventQueue<LogMessage*> processedLogMessages_;
        };
    }
}

#endif // MONZZA_LOGGER_THREAD_HPP