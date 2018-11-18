#ifndef MONZZA_LOG_FUNCTIONALITY_HPP
#define MONZZA_LOG_FUNCTIONALITY_HPP

#include "local_logger.hpp"

namespace monzza {
    namespace logger {
        class LogFunctionality {
        public:
            bool setLogger( Logger* logger );
            bool setModuleName( const std::string& moduleName );
            bool setLogLevel( const LogLevel& logLevel );

            Logger* getLogger() const;
            std::string getModuleName() const;
            LogLevel getLogLevel() const;

            void resetModuleName();
            void resetLogLevel();
        protected:
            void criticalErrorMsg( const std::string& msg );
            void errorMsg( const std::string& msg );
            void notificationMsg( const std::string& msg );
            void warnMsg( const std::string& msg );
            void debugMsg( const std::string& msg );
        private:
            LocalLogger localLogger_;
        };
    }
}

#endif // MONZZA_LOG_FUNCTIONALITY_HPP