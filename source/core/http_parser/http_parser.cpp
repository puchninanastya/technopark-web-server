#include "http_parser.hpp"

#include <cstring>
#include <string>

using namespace monzza::core::http;

HttpParser::HttpParser() :
    state_( HttpParserState::WaitingForHttpRequestMethodStart ) {
    buffer_ = new Buffer();
    request_ = new HttpRequest();
}

HttpParser::~HttpParser() {
    delete buffer_;
}


bool HttpParser::setState( HttpParserState state ) {
    state_ = state;
    return true;
}

bool HttpParser::addData( uint8_t* buf, uint32_t bufSize ) {
    if ( bufSize <= 0 ) {
        return false;
    }

    buffer_->write( buf, bufSize );

    if ( parse() == Completed ) {
        return true;
    }
    return false;
}

HttpParsingResult HttpParser::parse() {

    HttpRequestHeaderItem* currentHeaderItem = nullptr;

    while ( !buffer_->noMoreData() ) {

        switch ( state_ ) {
            case WaitingForHttpRequestMethodStart:
                switch ( tryToParseHttpRequestMethod() ) {
                    case Completed:
                        setState( WaitingForUriStart );
                        break;
                    case NeedMoreData:
                        return NeedMoreData;
                    case ParsingError:
                        return ParsingError;
                }
                break;
            case WaitingForUriStart:
                switch ( tryToParseUri() ) {
                    case Completed:
                        setState( WaitingForH );
                        break;
                    case NeedMoreData:
                        return NeedMoreData;
                    case ParsingError:
                        return ParsingError;
                }
                break;
            case WaitingForH:
                if ( buffer_->getCurrentPosChar() != 'H' ) {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
                setState( WaitingForHT );
                break;
            case WaitingForHT:
                if ( buffer_->getCurrentPosChar() != 'T' ) {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
                setState( WaitingForHTT );
                break;
            case WaitingForHTT:
                if ( buffer_->getCurrentPosChar() != 'T' ) {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
                setState( WaitingForHTTP );
                break;
            case WaitingForHTTP:
                if ( buffer_->getCurrentPosChar() != 'P' ) {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
                setState( WaitingForVersionSlash );
                break;
            case WaitingForVersionSlash:
                if ( buffer_->getCurrentPosChar() != '/' ) {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
                setState( WaitingForVersionMajor );
                break;
            case WaitingForVersionMajor:
                if ( !isDigitChar( buffer_->getCurrentPosChar() ) ) {
                    return ParsingError;
                }
                request_->setVersionMajor( (int8_t) buffer_->getCurrentPosChar() );
                buffer_->increaseCurrentPos();
                setState( WaitingForVersionDotSeparation );
                break;
            case WaitingForVersionDotSeparation:
                if ( buffer_->getCurrentPosChar() != '.') {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
                setState( WaitingForVersionMinor );
                break;
            case WaitingForVersionMinor:
                if ( !isDigitChar( buffer_->getCurrentPosChar() ) ) {
                    return ParsingError;
                }
                request_->setVersionMinor( (int8_t) buffer_->getCurrentPosChar() );
                buffer_->increaseCurrentPos();
                setState( WaitingForRequestLineCRLF );
                break;
            case WaitingForRequestLineCRLF:
                if ( buffer_->getCurrentPosChar() != '\r' ) {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
                if ( buffer_->getCurrentPosChar() != '\n' ) {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
                setState( WaitingForRequestHeaderItemStart );
                break;
            case WaitingForRequestHeaderItemStart:
                if ( buffer_->getCurrentPosChar() != '\r' ) {
                    setState( WaitingForRequestHeaderItemName );
                } else {
                    setState( WaitingForEndRequestCRLF );
                }
                break;
            case WaitingForRequestHeaderItemName:
                currentHeaderItem = new HttpRequestHeaderItem();
                switch ( tryToParseRequestHeaderName( currentHeaderItem ) ) {
                    case Completed:
                        setState( WaitingForRequestHeaderItemValue );
                        break;
                    case NeedMoreData:
                        return NeedMoreData;
                    case ParsingError:
                        delete currentHeaderItem;
                        return ParsingError;
                }
                break;
            case WaitingForRequestHeaderItemValue:
                if ( !currentHeaderItem ) {
                    return ParsingError;
                }
                switch ( tryToParseRequestHeaderValue( currentHeaderItem ) ) {
                    case Completed:
                        request_->addHeaderItem( currentHeaderItem );
                        currentHeaderItem = nullptr;
                        setState( WaitingForRequestHeaderItemCRLF );
                        break;
                    case NeedMoreData:
                        return NeedMoreData;
                    case ParsingError:
                        delete currentHeaderItem;
                        return ParsingError;
                }
                break;
            case WaitingForRequestHeaderItemCRLF:
                if ( buffer_->getCurrentPosChar() != '\r' ) {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
                if ( buffer_->getCurrentPosChar() != '\n' ) {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
                setState( WaitingForRequestHeaderItemStart );
                break;
            case WaitingForEndRequestCRLF:
                if ( buffer_->getCurrentPosChar() != '\r' ) {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
                if ( buffer_->getCurrentPosChar() != '\n' ) {
                    return ParsingError;
                }
                buffer_->increaseCurrentPos();
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