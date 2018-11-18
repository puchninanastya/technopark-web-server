#include "log_message.hpp"

using namespace monzza::logger;

bool LogMessage::setLogMessageType( const LogMessageType& logMessageType ) {
	logMessageType_ = logMessageType;
	return true;
}

bool LogMessage::setTime() {
	std::time( &time_ );
	return true;
}

bool LogMessage::setFrom( const std::string& from ) {
	from_ = from;
	return true;
}

bool LogMessage::setText( const std::string& text ) {
	text_ = text;
	return true;
}

LogMessage::LogMessageType LogMessage::getLogMessageType() const {
	return logMessageType_;
}

std::time_t LogMessage::getTime() const {
	return time_;
}

std::string LogMessage::getFrom() const {
	return from_;
}

std::string LogMessage::getText() const {
	return text_;
}

std::string LogMessage::toString() const {
	std::string str;

	cpl::time::convertUnixToIso8601Time( getTime(), str );

	str += "  " ;

    std::string moduleName = getFrom();
    if ( moduleName.size() < 20 ) {
        while ( moduleName.size() <= 20 ) {
			moduleName.push_back( ' ' );
		}
    }
    str += moduleName + "  ";

	switch ( getLogMessageType() ) {
		case LogMessageType::CRITICAL_ERROR_MSG:
			str += "CRITICAL_ERROR   ";
			break;
		case LogMessageType::ERROR_MSG:
			str += "ERROR            ";
			break;
		case LogMessageType::WARNING_MSG:
			str += "WARNING          ";
			break;
		case LogMessageType::NOTIFICATION_MSG:
			str += "NOTIFICATION     ";
			break;
		case LogMessageType::DEBUG_MSG:
			str += "DEBUG            ";
			break;
	}


	std::string logMsgText = getText();
	std::string tabStr;
	while ( tabStr.size() <= 66 ) {
		tabStr.push_back(' ');
	}

	std::stringstream ss;
	ss << str;

	uint32_t currentTextStringSize = 0;
	for ( auto& byte : logMsgText ) {
		if ( byte == '\n' ) {
			ss << byte << tabStr;
			currentTextStringSize = 0;
		}
		else {
			if ( currentTextStringSize <= 66 ) {
				ss << byte;
			}
			else {
				ss << std::endl << tabStr << byte;
				currentTextStringSize = 0;
			}
		}
		currentTextStringSize++;
	}

	return ss.str();
}