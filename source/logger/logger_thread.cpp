#include "logger_thread.hpp"

// =================================================================
// ========== LoggerThreadServiceMessage class definition ==========
// =================================================================

bool LoggerThreadServiceMessage::setResponseType( LoggerThreadServiceMessage::ResponseType responseType ) {
    responseType_ = responseType;
    return true;
}

LoggerThreadServiceMessage::ResponseType LoggerThreadServiceMessage::getResponseType() const {
    return responseType_;
}

bool LoggerThreadServiceMessage::setCommandType( LoggerThreadServiceMessage::CommandType commandType ) {
    commandType_ = commandType;
    return true;
}

LoggerThreadServiceMessage::CommandType LoggerThreadServiceMessage::getCommandType() const {
    return commandType_;
}

// ===================================================
// ========== LoggerThread class definition ==========
// ===================================================

LoggerThread::LoggerThread() {
    logLevel_ = LogLevel::LEVEL_1;

    spdLogger_ = spdlog::daily_logger_mt( "monzza_daily_logger", "logs/monzza.txt" );
    spdlog::set_pattern( "%v" );

    breakThreadLoop_ = false;

    events_.push_back( inputServiceMessages_.getEventHandle() );
    events_.push_back( logMessages_.getEventHandle() );
}

cpl::Event* LoggerThread::getNewOutputServiceMessageEvent() {
    return outputServiceMessages_.getEventHandle();
}

void LoggerThread::addInputServiceMessage( LoggerThreadServiceMessage* loggerThreadServiceMessage ) {
    inputServiceMessages_.push( loggerThreadServiceMessage );
}

LoggerThreadServiceMessage* LoggerThread::getOutputServiceMessage() {
    LoggerThreadServiceMessage* loggerThreadServiceMessage = nullptr;
    return outputServiceMessages_.tryPop( loggerThreadServiceMessage ) ? loggerThreadServiceMessage : nullptr;
}

cpl::Event* LoggerThread::getNewLogMessageEvent() {
    return processedLogMessages_.getEventHandle();
}

void LoggerThread::addLogMessage( LogMessage* logMessage ) {
    switch ( logMessage->getLogMessageType() ) {
        case LogMessage::LogMessageType::CRITICAL_ERROR_MSG:
            logMessages_.push( logMessage );
            break;
        case LogMessage::LogMessageType::ERROR_MSG:
            if ( logLevel_ != LogLevel::LEVEL_0 ) {
                logMessages_.push( logMessage );
            }
            break;
        case LogMessage::LogMessageType::NOTIFICATION_MSG:
            if ( logLevel_ != LogLevel::LEVEL_0 ) {
                logMessages_.push( logMessage );
            }
            break;
        case LogMessage::LogMessageType::WARNING_MSG:
            if ( ( logLevel_ == LogLevel::LEVEL_2 ) || ( logLevel_ == LogLevel::LEVEL_3 ) ) {
                logMessages_.push( logMessage );
            }
            break;
        case LogMessage::LogMessageType::DEBUG_MSG:
            if ( logLevel_ == LogLevel::LEVEL_3 ) {
                logMessages_.push( logMessage );
            }
            break;
    }
}

LogMessage* LoggerThread::getLogMessage() {
    LogMessage* logMessage = nullptr;
    return processedLogMessages_.tryPop( logMessage ) ? logMessage : nullptr;
}

void LoggerThread::operator()() {
    uint32_t waitResult = 0;

    while ( !breakThreadLoop_ ) {
        waitResult = cpl::EventExpectant::waitForEvents( &events_, false, CPL_EE_WFE_INFINITE_WAIT );
        switch ( waitResult ) {
            case 0:	// New service message.
                processInputServiceMessage();
                break;
            case 1: // New log messages.
                processLogMessages();
            default:
                break;
        }
    }
}

void LoggerThread::processInputServiceMessage() {
    LoggerThreadServiceMessage* loggerThreadServiceMessage;
    inputServiceMessages_.tryPop( loggerThreadServiceMessage );
    serviceMsgHandler( loggerThreadServiceMessage );
    outputServiceMessages_.push( loggerThreadServiceMessage );
}

void LoggerThread::processLogMessages() {
    LogMessage* logMessage = nullptr;

    while( logMessages_.tryPop( logMessage ) ) {
        std::cout << logMessage->toString() << std::endl;
        spdLogger_->info( logMessage->toString() );
        delete logMessage;
    }
}

void LoggerThread::serviceMsgHandler( LoggerThreadServiceMessage* loggerThreadServiceMessage ) {
    if( loggerThreadServiceMessage == nullptr ) {
        return;
    }

    switch( loggerThreadServiceMessage->getCommandType() ) {
        case LoggerThreadServiceMessage::CommandType::STOP:
            stopServiceMsgHandler( loggerThreadServiceMessage );
            break;
    }
}

void LoggerThread::stopServiceMsgHandler( LoggerThreadServiceMessage* loggerThreadServiceMessage ) {
    loggerThreadServiceMessage->setResponseType( LoggerThreadServiceMessage::ResponseType::SUCCESSFUL );
    breakThreadLoop_ = true;
}