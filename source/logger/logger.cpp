#include "logger.hpp"

using namespace monzza::logger;

bool Logger::start() {
    loggerThread_ = new LoggerThread();
    thread_ = new std::thread( std::ref( *(loggerThread_) ) );
    return true;
}

cpl::Event* Logger::getNewLogMessageEvent() {
    if ( loggerThread_ != nullptr ) {
        return loggerThread_->getNewLogMessageEvent();
    }
    else {
        return nullptr;
    }
}

void Logger::addLogMessage( LogMessage* logMessage ) {
    if ( loggerThread_ != nullptr ) {
        loggerThread_->addLogMessage( logMessage );
    }
}

LogMessage* Logger::getLogMessage() {
    if ( loggerThread_ != nullptr ) {
        return loggerThread_->getLogMessage();
    }
    else {
        return nullptr;
    }

}

bool Logger::stop() {
    if ( ( thread_ == nullptr ) && ( loggerThread_ == nullptr ) ) {
        return true;
    }

    auto loggerThreadServiceMessage = new LoggerThreadServiceMessage;

    loggerThreadServiceMessage->setCommandType( LoggerThreadServiceMessage::CommandType::STOP );
    loggerThread_->addInputServiceMessage( loggerThreadServiceMessage );
    cpl::Event* outputServiceMessageEvent = loggerThread_->getNewOutputServiceMessageEvent();
    cpl::EventExpectant::waitForEvent( outputServiceMessageEvent, CPL_EE_WFE_INFINITE_WAIT );
    loggerThreadServiceMessage = loggerThread_->getOutputServiceMessage();
    LoggerThreadServiceMessage::ResponseType responseType = loggerThreadServiceMessage->getResponseType();

    delete loggerThreadServiceMessage;

    if ( responseType == LoggerThreadServiceMessage::ResponseType::SUCCESSFUL ) {
        thread_->join();
        delete thread_;
        thread_ = nullptr;
        delete loggerThread_;
        loggerThread_ = nullptr;
        return true;
    }
    else {
        return false;
    }
}