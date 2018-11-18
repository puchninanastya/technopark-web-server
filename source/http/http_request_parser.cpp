#include "http_request_parser.hpp"

using namespace monzza::http;

HttpRequestParser::HttpRequestParser() :
    state_( HttpRequestParserState::WAITING_FOR_HTTP_REQUEST_METHOD_START )
{
    buffer_ = new monzza::buffer::Buffer();
    request_ = new HttpRequest();
}

HttpRequestParser::~HttpRequestParser() {
    delete buffer_;
    buffer_ = nullptr;
    delete request_;
    request_ = nullptr;
}

bool HttpRequestParser::initialize( monzza::logger::Logger* logger, std::string parentModuleName ) {
    if ( logger == nullptr ) {
        return false;
    }

    setLogger( logger );
    setModuleName( parentModuleName );

    return true;
}


bool HttpRequestParser::setState( HttpRequestParserState state ) {
    state_ = state;
    return true;
}

bool HttpRequestParser::addDataAndParse( uint8_t* buf, uint32_t bufSize ) {
    if ( bufSize <= 0 ) {
        return false;
    }

    std::string buffer( ( char* )buf, bufSize );
    debugMsg( "Adding data to parser:\n" + buffer );

    buffer_->write( buf, bufSize );

    if ( parse() == HttpParsingResult::COMPLETED ) {
        notificationMsg( "Parsing completed successfully." );
        return true;
    }
    else {
        notificationMsg( "Parsing failed." );
        return false;
    }
}

bool HttpRequestParser::cleanData() {
    buffer_->clean();

    delete request_;
    request_ = nullptr;
    request_ = new HttpRequest();

    setState( WAITING_FOR_HTTP_REQUEST_METHOD_START );

    notificationMsg( "Data has been cleaned." );
}

HttpParsingResult HttpRequestParser::parse() {
    notificationMsg( "Parsing HttpRequest." );
    HttpHeaderItem* currentHeaderItem = nullptr;

    while ( !buffer_->noMoreData() ) {
        switch ( state_ ) {
            case WAITING_FOR_HTTP_REQUEST_METHOD_START:
                debugMsg( "WAITING_FOR_HTTP_REQUEST_METHOD_START." );

                switch ( tryToParseHttpRequestMethod() ) {
                    case HttpParsingResult::COMPLETED:
                        setState( WAITING_FOR_URI_START );
                        debugMsg( "WAITING_FOR_HTTP_REQUEST_METHOD_START COMPLETED." );
                        break;
                    case HttpParsingResult::NEED_MORE_DATA:
                        return HttpParsingResult::NEED_MORE_DATA;
                    case HttpParsingResult::PARSING_ERROR:
                        return HttpParsingResult::PARSING_ERROR;
                }

                break;
            case WAITING_FOR_URI_START:
                debugMsg( "WAITING_FOR_URI_START." );

                switch ( tryToParseUri() ) {
                    case HttpParsingResult::COMPLETED:
                        setState( WAITING_FOR_H );
                        debugMsg( "WAITING_FOR_URI_START COMPLETED." );
                        break;
                    case HttpParsingResult::NEED_MORE_DATA:
                        return HttpParsingResult::NEED_MORE_DATA;
                    case HttpParsingResult::PARSING_ERROR:
                        return HttpParsingResult::PARSING_ERROR;
                }

                break;
            case WAITING_FOR_H:
                debugMsg( "WAITING_FOR_H." );

                if ( buffer_->getCurrentPosChar() != 'H' ) {
                    return HttpParsingResult::PARSING_ERROR;
                }

                buffer_->increaseCurrentPos();
                setState( WAITING_FOR_HT );

                debugMsg( "WAITING_FOR_H COMPLETED." );

                break;
            case WAITING_FOR_HT:
                debugMsg( "WAITING_FOR_HT." );

                if ( buffer_->getCurrentPosChar() != 'T' ) {
                    return HttpParsingResult::PARSING_ERROR;
                }

                buffer_->increaseCurrentPos();
                setState( WAITING_FOR_HTT );

                debugMsg( "WAITING_FOR_HT COMPLETED." );

                break;
            case WAITING_FOR_HTT:
                debugMsg( "WAITING_FOR_HTT." );

                if ( buffer_->getCurrentPosChar() != 'T' ) {
                    return HttpParsingResult::PARSING_ERROR;
                }

                buffer_->increaseCurrentPos();
                setState( WAITING_FOR_HTTP );

                debugMsg( "WAITING_FOR_HTT COMPLETED." );

                break;
            case WAITING_FOR_HTTP:
                debugMsg( "WAITING_FOR_HTTP." );

                if ( buffer_->getCurrentPosChar() != 'P' ) {
                    return HttpParsingResult::PARSING_ERROR;
                }

                buffer_->increaseCurrentPos();
                setState( WAITING_FOR_VERSION_SLASH );

                debugMsg( "WAITING_FOR_HTTP COMPLETED." );

                break;
            case WAITING_FOR_VERSION_SLASH:
                debugMsg( "WAITING_FOR_VERSION_SLASH." );

                if ( buffer_->getCurrentPosChar() != '/' ) {
                    return HttpParsingResult::PARSING_ERROR;
                }

                buffer_->increaseCurrentPos();
                setState( WAITING_FOR_VERSION_MAJOR );
                debugMsg( "WAITING_FOR_VERSION_SLASH COMPLETED." );

                break;
            case WAITING_FOR_VERSION_MAJOR:
                debugMsg( "WAITING_FOR_VERSION_MAJOR." );

                if ( !isDigitChar( buffer_->getCurrentPosChar() ) ) {
                    return HttpParsingResult::PARSING_ERROR;
                }

                request_->setVersionMajor( ( int8_t )buffer_->getCurrentPosChar() );
                buffer_->increaseCurrentPos();
                setState( WAITING_FOR_VERSION_DOT_SEPARATION );
                debugMsg( "WAITING_FOR_VERSION_MAJOR COMPLETED." );

                break;
            case WAITING_FOR_VERSION_DOT_SEPARATION:
                debugMsg( "WAITING_FOR_VERSION_DOT_SEPARATION." );

                if ( buffer_->getCurrentPosChar() != '.') {
                    return HttpParsingResult::PARSING_ERROR;
                }

                buffer_->increaseCurrentPos();
                setState( WAITING_FOR_VERSION_MINOR );
                debugMsg( "WAITING_FOR_VERSION_DOT_SEPARATION COMPLETED." );

                break;
            case WAITING_FOR_VERSION_MINOR:
                debugMsg( "WAITING_FOR_VERSION_MINOR." );

                if ( !isDigitChar( buffer_->getCurrentPosChar() ) ) {
                    return HttpParsingResult::PARSING_ERROR;
                }

                request_->setVersionMinor( ( int8_t )buffer_->getCurrentPosChar() );
                buffer_->increaseCurrentPos();
                setState( WAITING_FOR_REQUEST_LINE_CRLF );

                debugMsg( "WAITING_FOR_VERSION_MINOR COMPLETED." );

                break;
            case WAITING_FOR_REQUEST_LINE_CRLF:
                debugMsg( "WAITING_FOR_REQUEST_LINE_CRLF." );

                if ( buffer_->getCurrentPosChar() != '\r' ) {
                    return HttpParsingResult::PARSING_ERROR;
                }

                buffer_->increaseCurrentPos();

                if ( buffer_->getCurrentPosChar() != '\n' ) {
                    return HttpParsingResult::PARSING_ERROR;
                }

                buffer_->increaseCurrentPos();
                setState( WAITING_FOR_REQUEST_HEADER_ITEM_START );

                debugMsg( "WAITING_FOR_REQUEST_LINE_CRLF COMPLETED." );

                break;
            case WAITING_FOR_REQUEST_HEADER_ITEM_START:
                debugMsg( "WAITING_FOR_REQUEST_HEADER_ITEM_START." );

                if ( buffer_->getCurrentPosChar() != '\r' ) {
                    setState( WAITING_FOR_REQUEST_HEADER_ITEM_NAME );
                }
                else {
                    setState( WAITING_FOR_END_REQUEST_CRLF );
                }

                break;
            case WAITING_FOR_REQUEST_HEADER_ITEM_NAME:
                debugMsg( "WAITING_FOR_REQUEST_HEADER_ITEM_NAME." );
                currentHeaderItem = new HttpHeaderItem();

                switch ( tryToParseRequestHeaderName( currentHeaderItem ) ) {
                    case HttpParsingResult::COMPLETED:
                        setState( WAITING_FOR_REQUEST_HEADER_ITEM_VALUE );
                        debugMsg( "WAITING_FOR_REQUEST_HEADER_ITEM_NAME COMPLETED." );
                        break;
                    case HttpParsingResult::NEED_MORE_DATA:
                        return HttpParsingResult::NEED_MORE_DATA;
                    case HttpParsingResult::PARSING_ERROR:
                        delete currentHeaderItem;
                        currentHeaderItem = nullptr;
                        return HttpParsingResult::PARSING_ERROR;
                }

                break;
            case WAITING_FOR_REQUEST_HEADER_ITEM_VALUE:
                debugMsg( "WAITING_FOR_REQUEST_HEADER_ITEM_VALUE." );
                if ( !currentHeaderItem ) {
                    return HttpParsingResult::PARSING_ERROR;
                }

                switch ( tryToParseRequestHeaderValue( currentHeaderItem ) ) {
                    case HttpParsingResult::COMPLETED:
                        request_->addHeaderItem( currentHeaderItem );
                        currentHeaderItem = nullptr;
                        setState( WAITING_FOR_REQUEST_HEADER_ITEM_CRLF );
                        debugMsg( "WAITING_FOR_REQUEST_HEADER_ITEM_VALUE COMPLETED." );
                        break;
                    case HttpParsingResult::NEED_MORE_DATA:
                        return HttpParsingResult::NEED_MORE_DATA;
                    case HttpParsingResult::PARSING_ERROR:
                        delete currentHeaderItem;
                        currentHeaderItem = nullptr;
                        return HttpParsingResult::PARSING_ERROR;
                }

                break;
            case WAITING_FOR_REQUEST_HEADER_ITEM_CRLF:
                debugMsg( "WAITING_FOR_REQUEST_HEADER_ITEM_CRLF." );

                if ( buffer_->getCurrentPosChar() != '\r' ) {
                    return HttpParsingResult::PARSING_ERROR;
                }

                buffer_->increaseCurrentPos();

                if ( buffer_->getCurrentPosChar() != '\n' ) {
                    return HttpParsingResult::PARSING_ERROR;
                }

                buffer_->increaseCurrentPos();
                setState( WAITING_FOR_REQUEST_HEADER_ITEM_START );
                debugMsg( "WAITING_FOR_REQUEST_HEADER_ITEM_CRLF COMPLETED." );

                break;
            case WAITING_FOR_END_REQUEST_CRLF:
                debugMsg( "WAITING_FOR_END_REQUEST_CRLF." );

                if ( buffer_->getCurrentPosChar() != '\r' ) {
                    return HttpParsingResult::PARSING_ERROR;
                }

                buffer_->increaseCurrentPos();

                if ( buffer_->getCurrentPosChar() != '\n' ) {
                    return HttpParsingResult::PARSING_ERROR;
                }

                buffer_->increaseCurrentPos();
                setState( WAITING_FOR_REQUEST_HEADER_ITEM_START );
                debugMsg( "WAITING_FOR_END_REQUEST_CRLF COMPLETED." );

                return HttpParsingResult::COMPLETED;
            default:
                break;
        }
    }

    return HttpParsingResult::COMPLETED;
}

bool HttpRequestParser::isDigitChar( uint8_t bufChar ) {
    return ( ( bufChar >= '0' ) && ( bufChar <= '9' ) );
}

uint32_t HttpRequestParser::skipBytesUntilSymbol( char c ) {
    uint8_t currentChar = buffer_->getCurrentPosChar();
    uint32_t pos = buffer_->getCurrentPos();
    uint32_t readSize = 0;

    while ( buffer_->checkAvailableDataSizeFromPos( pos ) && currentChar ) {
        if ( currentChar == c ) {
            buffer_->increaseCurrentPos( readSize );
            return readSize;
        }

        readSize++;
        pos++;
        currentChar = buffer_->getPosChar( pos );
    }
}

HttpParsingResult HttpRequestParser::tryToParseHttpRequestMethod() {
    int32_t startReadingPos = buffer_->getCurrentPos();
    uint32_t readSize = skipBytesUntilSymbol( ' ' );

    if ( !readSize ) {
        return HttpParsingResult::NEED_MORE_DATA;
    }

    buffer_->increaseCurrentPos();
    if ( buffer_->compareToString( startReadingPos, "GET", readSize ) == 0 ) {
        request_->setHttpMethod( HttpRequestMethod::GET );
        return HttpParsingResult::COMPLETED;
    }
    else if ( buffer_->compareToString( startReadingPos, "HEAD", readSize ) == 0 ) {
        request_->setHttpMethod( HttpRequestMethod::HEAD );
        return HttpParsingResult::COMPLETED;
    }
    else if ( buffer_->compareToString( startReadingPos, "POST", readSize ) == 0 ) {
        request_->setHttpMethod( HttpRequestMethod::POST );
        return HttpParsingResult::COMPLETED;
    } // TODO: add NOT_IMPLEMETNED for other methods


    return HttpParsingResult::PARSING_ERROR;
}

HttpParsingResult HttpRequestParser::tryToParseUri() {
    int32_t startReadingPos = buffer_->getCurrentPos();
    uint32_t readSize = skipBytesUntilSymbol( ' ' );

    if ( !readSize ) {
        return HttpParsingResult::NEED_MORE_DATA;
    }

    buffer_->increaseCurrentPos();
    std::string temp_uri;
    buffer_->copyToString( temp_uri, startReadingPos, readSize );
    std::string url = urlDecode( temp_uri );
    request_->setUri( getPathFromUrl( url ) );

    return HttpParsingResult::COMPLETED;
}

std::string HttpRequestParser::urlDecode( std::string encodedUrl ) {
    std::string decodedUrl;
    char ch;
    int i, j;
    for ( i = 0; i < encodedUrl.length(); i++ ) {
        if ( int( encodedUrl[ i ] ) == 37 ) {
            sscanf( encodedUrl.substr( i + 1, 2 ).c_str(), "%x", &j );
            ch = static_cast<char>( j );
            decodedUrl += ch;
            i = i + 2;
        } else {
            decodedUrl += encodedUrl[ i ];
        }
    }
    return decodedUrl;
}

std::string HttpRequestParser::getPathFromUrl(const std::string &url) {
    return url.substr(0, url.find_last_of('?'));
}

HttpParsingResult HttpRequestParser::tryToParseRequestHeaderName( HttpHeaderItem* item ) {
    int32_t startReadingPos = buffer_->getCurrentPos();
    uint32_t readSize = skipBytesUntilSymbol( ' ' );

    if ( !readSize ) {
        return HttpParsingResult::NEED_MORE_DATA;
    }

    buffer_->increaseCurrentPos();
    buffer_->copyToString( item->name, startReadingPos, ( readSize - 1 ) );

    return HttpParsingResult::COMPLETED;
}

HttpParsingResult HttpRequestParser::tryToParseRequestHeaderValue( HttpHeaderItem* item ) {
    int32_t startReadingPos = buffer_->getCurrentPos();
    uint32_t readSize = skipBytesUntilSymbol( '\r' );

    if ( !readSize ) {
        return HttpParsingResult::NEED_MORE_DATA;
    }

    buffer_->copyToString( item->value, startReadingPos, readSize );

    return HttpParsingResult::COMPLETED;
}

HttpRequest* HttpRequestParser::getHttpRequest() {
    return request_;
}
