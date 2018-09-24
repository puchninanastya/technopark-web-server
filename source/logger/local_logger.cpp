#include "local_logger.hpp"

LocalLogger::LocalLogger() :
	logLevel_( LogLevel::LEVEL_1 ),
	logger_( nullptr )
{}

LocalLogger::LocalLogger( Logger* logger ) {
	logLevel_ = LogLevel::LEVEL_1;
	logger_ = logger;
}

bool LocalLogger::setLogger( Logger* logger ) {
	logger_ = logger;
	return true;
}

Logger* LocalLogger::getLogger() const {
	return logger_;
}

bool LocalLogger::setModuleName( const std::string& moduleName ) {
	moduleName_ = moduleName;
	return true;
}

std::string LocalLogger::getModuleName() const {
	return moduleName_;
}

void LocalLogger::resetModuleName() {
	moduleName_ = std::string( "" );
}

bool LocalLogger::setLogLevel( const LogLevel& logLevel ) {
	logLevel_ = logLevel;
	return true;
}

LogLevel LocalLogger::getLogLevel() const {
	return logLevel_;
}

void LocalLogger::resetLogLevel() {
	logLevel_ = LogLevel::LEVEL_0;
}

void LocalLogger::criticalErrorMsg( const std::string& msg ) {
	LogMessage* logMessage = nullptr;
	if ( logger_ != nullptr ) {
		logMessage = new LogMessage;
		logMessage->setLogMessageType( LogMessage::LogMessageType::CRITICAL_ERROR_MSG );
		logMessage->setTime();
		logMessage->setFrom( moduleName_ );
		logMessage->setText( msg );
		logger_->addLogMessage( logMessage );
	}
	else {
		std::cout << msg << std::endl;
	}
}

void LocalLogger::errorMsg( const std::string& msg ) {
	LogMessage* logMessage = nullptr;
	if ( logLevel_ != LogLevel::LEVEL_0 ) {
		if ( logger_ != nullptr ) {
			logMessage = new LogMessage;
			logMessage->setLogMessageType( LogMessage::LogMessageType::ERROR_MSG );
			logMessage->setTime();
			logMessage->setFrom( moduleName_ );
			logMessage->setText( msg );
			logger_->addLogMessage( logMessage );
		}
		else {
			std::cout << msg << std::endl;
		}
	}
}

void LocalLogger::notificationMsg( const std::string& msg ) {
	LogMessage* logMessage = nullptr;
	if ( logLevel_ != LogLevel::LEVEL_0 ) {
		if ( logger_ != nullptr ) {
			logMessage = new LogMessage;
			logMessage->setLogMessageType( LogMessage::LogMessageType::NOTIFICATION_MSG );
			logMessage->setTime();
			logMessage->setFrom( moduleName_ );
			logMessage->setText( msg );
			logger_->addLogMessage( logMessage );
		}
		else {
			std::cout << msg << std::endl;
		}
	}
}

void LocalLogger::warnMsg( const std::string& msg ) {
	LogMessage* logMessage = nullptr;
	if ( ( logLevel_ == LogLevel::LEVEL_2 ) || ( logLevel_ == LogLevel::LEVEL_3) ) {
		if ( logger_ != nullptr ) {
			logMessage = new LogMessage;
			logMessage->setLogMessageType( LogMessage::LogMessageType::WARNING_MSG );
			logMessage->setTime();
			logMessage->setFrom( moduleName_ );
			logMessage->setText( msg );
			logger_->addLogMessage( logMessage );
		}
		else {
			std::cout << msg << std::endl;
		}
	}
}

void LocalLogger::debugMsg( const std::string& msg ) {
	LogMessage* logMessage = nullptr;
	if ( logLevel_ == LogLevel::LEVEL_3 ) {
		if ( logger_ != nullptr ) {
			logMessage = new LogMessage;
			logMessage->setLogMessageType( LogMessage::LogMessageType::DEBUG_MSG );
			logMessage->setTime();
			logMessage->setFrom( moduleName_ );
			logMessage->setText( msg );
			logger_->addLogMessage( logMessage );
		}
		else {
			std::cout << msg << std::endl;
		}
	}
}