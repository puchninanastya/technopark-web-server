#include "http_parser.hpp"

using namespace monzza::http;

HttpParser::HttpParser() :
    state_( HttpParserState::WaitingForHttpRequestMethodStart ) {
    buffer_ = new Buffer();
    request_ = new HttpRequest();
}

HttpParser::~HttpParser() {
    delete buffer_;
    delete request_;
}

bool HttpParser::initialize( Logger* logger ) {
    if ( logger == nullptr ) {
        return false;
    }

    setLogger( logger );
    std::string moduleName( "HttpParser" );
    setModuleName( moduleName );

    return true;
}


bool HttpParser::setState( HttpParserState state ) {
    state_ = state;
    return true;
}

bool HttpParser::addData( uint8_t* buf, uint32_t bufSize ) {
    if ( bufSize <= 0 ) {
        return false;
    }

    notificationMsg( "Adding data to parser." );
    std::cout << std::endl;
    for ( int i = 0; i < bufSize; i++ ) {
        std::cout << buf[i];
    }
    std::cout << std::endl;

    buffer_->write( buf, bufSize );

    if ( parse() == Completed ) {
        notificationMsg( "Parsing completed successfully." );
        return true;
    }
    notificationMsg( "Parsing failed." );
    return false;
}

HttpParsingResult HttpParser::parse() {

    notificationMsg( "Parsing HttpRequest." );
    HttpRequestHeaderItem* currentHeaderItem = nullptr;

    while ( !buffer_->noMoreData() ) {

        switch ( state_ ) {
            case WaitingForHttpRequestMethodStart:
                notificationMsg( "WaitingForHttpRequestMethodStart." );
                switch ( tryToParseHttpRequestMethod() ) {
                    case Completed:
                        setState( WaitingForUriStart );
                        notificationMsg( "WaitingForHttpRequestMethodStart Completed." );
                        break;
                    case NeedMoreData:
                        return NeedMoreData;
                    case ParsingError:
                        return ParsingError;
                }
                break;
            case WaitingForUriStart:
                notificationMsg( "WaitingForUriStart." );
                switch ( tryToParseUri() ) {
                    case Completed:
                        setState( WaitingForH );
                        notificationMsg( "WaitingForUriStart Completed." );
                        break;
                    case NeedMoreData:
                        return NeedMoreData;
                    case ParsingError:
                        return ParsingError;
                }
                break;
            case WaitingForH:
                notificationMsg( "WaitingForH." );
                if ( buffer_->getCurrentPosChar() != 'H' ) {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
                setState( WaitingForHT );
                notificationMsg( "WaitingForH Completed." );
                break;
            case WaitingForHT:
                notificationMsg( "WaitingForHT." );
                if ( buffer_->getCurrentPosChar() != 'T' ) {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
                setState( WaitingForHTT );
                notificationMsg( "WaitingForHT Completed." );
                break;
            case WaitingForHTT:
                notificationMsg( "WaitingForHTT." );
                if ( buffer_->getCurrentPosChar() != 'T' ) {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
                setState( WaitingForHTTP );
                notificationMsg( "WaitingForHTT Completed." );
                break;
            case WaitingForHTTP:
                notificationMsg( "WaitingForHTTP." );
                if ( buffer_->getCurrentPosChar() != 'P' ) {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
                setState( WaitingForVersionSlash );
                notificationMsg( "WaitingForHTTP Completed." );
                break;
            case WaitingForVersionSlash:
                notificationMsg( "WaitingForVersionSlash." );
                if ( buffer_->getCurrentPosChar() != '/' ) {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
                setState( WaitingForVersionMajor );
                notificationMsg( "WaitingForVersionSlash Completed." );
                break;
            case WaitingForVersionMajor:
                notificationMsg( "WaitingForVersionMajor." );
                if ( !isDigitChar( buffer_->getCurrentPosChar() ) ) {
                    return ParsingError;
                }
                request_->setVersionMajor( (int8_t) buffer_->getCurrentPosChar() );
                buffer_->increaseCurrentPos();
                setState( WaitingForVersionDotSeparation );
                notificationMsg( "WaitingForVersionMajor Completed." );
                break;
            case WaitingForVersionDotSeparation:
                notificationMsg( "WaitingForVersionDotSeparation." );
                if ( buffer_->getCurrentPosChar() != '.') {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
                setState( WaitingForVersionMinor );
                notificationMsg( "WaitingForVersionDotSeparation Completed." );
                break;
            case WaitingForVersionMinor:
                notificationMsg( "WaitingForVersionMinor." );
                if ( !isDigitChar( buffer_->getCurrentPosChar() ) ) {
                    return ParsingError;
                }
                request_->setVersionMinor( (int8_t) buffer_->getCurrentPosChar() );
                buffer_->increaseCurrentPos();
                setState( WaitingForRequestLineCRLF );
                notificationMsg( "WaitingForVersionMinor Completed." );
                break;
            case WaitingForRequestLineCRLF:
                notificationMsg( "WaitingForRequestLineCRLF." );
                if ( buffer_->getCurrentPosChar() != '\r' ) {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
                if ( buffer_->getCurrentPosChar() != '\n' ) {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
                setState( WaitingForRequestHeaderItemStart );
                notificationMsg( "WaitingForRequestLineCRLF Completed." );
                break;
            case WaitingForRequestHeaderItemStart:
                notificationMsg( "WaitingForRequestHeaderItemStart." );
                if ( buffer_->getCurrentPosChar() != '\r' ) {
                    setState( WaitingForRequestHeaderItemName );
                } else {
                    setState( WaitingForEndRequestCRLF );
                }
                break;
            case WaitingForRequestHeaderItemName:
                notificationMsg( "WaitingForRequestHeaderItemName." );
                currentHeaderItem = new HttpRequestHeaderItem();
                switch ( tryToParseRequestHeaderName( currentHeaderItem ) ) {
                    case Completed:
                        setState( WaitingForRequestHeaderItemValue );
                        notificationMsg( "WaitingForRequestHeaderItemName Completed." );
                        break;
                    case NeedMoreData:
                        return NeedMoreData;
                    case ParsingError:
                        delete currentHeaderItem;
                        return ParsingError;
                }
                break;
            case WaitingForRequestHeaderItemValue:
                notificationMsg( "WaitingForRequestHeaderItemValue." );
                if ( !currentHeaderItem ) {
                    return ParsingError;
                }
                switch ( tryToParseRequestHeaderValue( currentHeaderItem ) ) {
                    case Completed:
                        request_->addHeaderItem( currentHeaderItem );
                        currentHeaderItem = nullptr;
                        setState( WaitingForRequestHeaderItemCRLF );
                        notificationMsg( "WaitingForRequestHeaderItemValue Completed." );
                        break;
                    case NeedMoreData:
                        return NeedMoreData;
                    case ParsingError:
                        delete currentHeaderItem;
                        return ParsingError;
                }
                break;
            case WaitingForRequestHeaderItemCRLF:
                notificationMsg( "WaitingForRequestHeaderItemCRLF." );
                if ( buffer_->getCurrentPosChar() != '\r' ) {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
                if ( buffer_->getCurrentPosChar() != '\n' ) {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
                setState( WaitingForRequestHeaderItemStart );
                notificationMsg( "WaitingForRequestHeaderItemCRLF Completed." );
                break;
            case WaitingForEndRequestCRLF:
                notificationMsg( "WaitingForEndRequestCRLF." );
                if ( buffer_->getCurrentPosChar() != '\r' ) {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
                if ( buffer_->getCurrentPosChar() != '\n' ) {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
                notificationMsg( "WaitingForEndRequestCRLF Completed." );
                return Completed;
            default:
                break;
        }
    }

    return HttpParsingResult::Completed;
}

bool HttpParser::isSymbolChar( uint8_t bufChar ) {
    return ( bufChar >= 0 && bufChar <= 127 );
}

bool HttpParser::isControlChar( uint8_t bufChar ) {
    return ( bufChar >= 0 && bufChar <= 31 ) || ( bufChar == 127 );
}

bool HttpParser::isSpecialChar( uint8_t bufChar ) {
    switch ( bufChar ) {
        case '(': case ')': case '<': case '>': case '@':
        case ',': case ';': case ':': case '\\': case '"':
        case '/': case '[': case ']': case '?': case '=':
        case '{': case '}': case ' ': case '\t':
            return true;
        default:
            return false;
    }
}

bool HttpParser::isDigitChar( uint8_t bufChar ) {
    return ( bufChar >= '0' && bufChar <= '9' );
}

uint32_t HttpParser::skipBytesUntilSymbol( char c ) {
    uint8_t currentChar = buffer_->getCurrentPosChar();
    uint32_t pos = buffer_->getCurrentPos();
    uint32_t readSize = 0;
    while ( buffer_->checkAvailableDataSizeFromPos( pos ) && currentChar ) {
        if ( currentChar == c ) {
            buffer_->increaseCurrentPos( readSize );
            return readSize;
        }
        readSize++;
        pos++; // TODO: check if not the end of buffer
        currentChar = buffer_->getPosChar( pos );
    }

}


HttpParsingResult HttpParser::tryToParseHttpRequestMethod() {
    int32_t startReadingPos = buffer_->getCurrentPos();
    uint32_t readSize = skipBytesUntilSymbol( ' ' );

    if ( !readSize ) {
        // TODO: check currentPos
        return NeedMoreData;
    }

    buffer_->increaseCurrentPos();
    if ( buffer_->compareToString( startReadingPos, "GET", readSize ) == 0 ) {
        request_->setHttpMethod( HttpRequestMethod::GET );
        return Completed;
    } else if ( buffer_->compareToString( startReadingPos, "HEAD", readSize ) == 0 ) {
        request_->setHttpMethod( HttpRequestMethod::HEAD );
        return Completed;
    } // TODO: add other methods


    return ParsingError;
}

HttpParsingResult HttpParser::tryToParseUri() {
    int32_t startReadingPos = buffer_->getCurrentPos();
    uint32_t readSize = skipBytesUntilSymbol( ' ' );

    if ( !readSize ) {
        // TODO: check currentPos
        return NeedMoreData;
    }

    buffer_->increaseCurrentPos();
    std::string uri;
    buffer_->copyToString( uri, startReadingPos, readSize );
    request_->setUri( uri );
    return Completed;

    // TODO: check if contain control chars
}

HttpParsingResult HttpParser::tryToParseRequestHeaderName( HttpRequestHeaderItem* item ) {
    int32_t startReadingPos = buffer_->getCurrentPos();
    uint32_t readSize = skipBytesUntilSymbol( ' ' );

    if ( !readSize ) {
        // TODO: check currentPos
        return NeedMoreData;
    }

    buffer_->increaseCurrentPos();
    buffer_->copyToString( item->name, startReadingPos, readSize - 1);
    return Completed;

    // TODO: check if contain control chars
}

HttpParsingResult HttpParser::tryToParseRequestHeaderValue( HttpRequestHeaderItem* item ) {
    int32_t startReadingPos = buffer_->getCurrentPos();
    uint32_t readSize = skipBytesUntilSymbol( '\r' );

    if ( !readSize ) {
        // TODO: check currentPos
        return NeedMoreData;
    }

    buffer_->copyToString( item->value, startReadingPos, readSize );
    return Completed;

    // TODO: check if contain control chars
}