#include "log_functionality.hpp"

bool LogFunctionality::setLogger( Logger* logger ) {
	return localLogger_.setLogger( logger );
}

bool LogFunctionality::setModuleName( const std::string& moduleName ) {
	return localLogger_.setModuleName( moduleName );
}

bool LogFunctionality::setLogLevel( const LogLevel& logLevel ) {
	return localLogger_.setLogLevel( logLevel );
}

Logger* LogFunctionality::getLogger() const {
	return localLogger_.getLogger();
}

std::string LogFunctionality::getModuleName() const {
	return localLogger_.getModuleName();
}

LogLevel LogFunctionality::getLogLevel() const {
	return localLogger_.getLogLevel();
}

void LogFunctionality::resetModuleName() {
	localLogger_.resetLogLevel();
}

void LogFunctionality::resetLogLevel() {
	localLogger_.resetLogLevel();
}

void LogFunctionality::criticalErrorMsg( const std::string& msg ) {
	localLogger_.criticalErrorMsg( msg );
}

void LogFunctionality::errorMsg( const std::string& msg ) {
	localLogger_.errorMsg( msg );
}

void LogFunctionality::notificationMsg( const std::string& msg ) {
	localLogger_.notificationMsg( msg );
}

void LogFunctionality::warnMsg( const std::string& msg ) {
	localLogger_.warnMsg( msg );
}

void LogFunctionality::debugMsg( const std::string& msg ) {
	localLogger_.debugMsg( msg );
}