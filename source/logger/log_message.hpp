#ifndef MONZZA_LOG_MESSAGE_HPP
#define MONZZA_LOG_MESSAGE_HPP

#include <ctime>
#include <string>
#include <sstream>
#include <cstdint>

#include "../../external/cpl/cpl_time.hpp"

class LogMessage {
public:
    enum class LogMessageType {
        CRITICAL_ERROR_MSG,
        ERROR_MSG,
        NOTIFICATION_MSG,
        WARNING_MSG,
        DEBUG_MSG,
    };

    bool setLogMessageType( const LogMessageType& logMessageType );
    bool setTime();
    bool setFrom( const std::string& from );
    bool setText( const std::string& text );

    LogMessageType getLogMessageType() const;
    std::time_t getTime() const;
    std::string getFrom() const;
    std::string getText() const;

    std::string toString() const;
private:
    LogMessageType logMessageType_;
    std::time_t time_;
    std::string from_;
    std::string text_;
};

#endif // MONZZA_LOG_MESSAGE_HPP