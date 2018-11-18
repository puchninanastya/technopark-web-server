#ifndef MONZZA_LOCAL_LOGGER_HPP
#define MONZZA_LOCAL_LOGGER_HPP

#include <iostream>
#include <string>

#include "log_level.hpp"
#include "log_message.hpp"
#include "logger.hpp"

namespace monzza {
    namespace logger {
        class LocalLogger {
        public:
            LocalLogger();
            explicit LocalLogger( Logger* logger );

            bool setLogger( Logger* logger );
            Logger* getLogger() const;

            bool setModuleName( const std::string& moduleName );
            std::string getModuleName() const;

            void resetModuleName();

            bool setLogLevel( const LogLevel& logLevel );
            LogLevel getLogLevel() const;

            void resetLogLevel();

            void criticalErrorMsg( const std::string& msg );
            void errorMsg( const std::string& msg );
            void notificationMsg( const std::string& msg );
            void warnMsg( const std::string& msg );
            void debugMsg( const std::string& msg );
        private:
            Logger* logger_;
            std::string moduleName_;
            LogLevel logLevel_;
        };
    }
}

#endif // MONZZA_LOCAL_LOGGER_HPP