#include <vector>

#include "http_response_serializer.hpp"

using namespace monzza::http;

HttpResponseSerializer::HttpResponseSerializer() {
    buffer_ = new monzza::buffer::Buffer();
    response_ = new HttpResponse();
}

HttpResponseSerializer::~HttpResponseSerializer() {
    delete buffer_;
    delete response_;
}

bool HttpResponseSerializer::initialize( monzza::logger::Logger* logger, std::string parentModuleName ) {
    if ( logger == nullptr ) {
        return false;
    }

    setLogger( logger );
    setModuleName( parentModuleName );

    return true;
}

void HttpResponseSerializer::initializeHttpResponse() {
    // Http protocol version
    response_->setVersionMajor(1);
    response_->setVersionMinor(1);

    // Basic headers
    response_->addHeaderItem( MONZZA_HTTP_HEADER_SERVER, "Monzza" ); // TODO: add name to configure?
    response_->addHeaderItem( MONZZA_HTTP_HEADER_CONNECTION, "Close" );
    response_->addHeaderItem( MONZZA_HTTP_HEADER_DATE, "Now" ); // TODO: add real current datetime
}

bool HttpResponseSerializer::createHttpResponseForExistingFile( HttpFileDescription httpFileDescription ) {
    initializeHttpResponse();
    response_->setStatusCode( 200 );

    response_->addHeaderItem( MONZZA_HTTP_HEADER_CONTENT_LENGTH,
                              std::to_string( httpFileDescription.fileSize ) );
    response_->addHeaderItem( MONZZA_HTTP_HEADER_CONTENT_TYPE,
                              getContentTypeForFilePath( httpFileDescription.fileType ) );
    return true;
}

bool HttpResponseSerializer::createHttpResponseForForbidden() {
    initializeHttpResponse();
    response_->setStatusCode( 403 );
    return true;
}

bool HttpResponseSerializer::createHttpResponseForNotFound() {
    initializeHttpResponse();
    response_->setStatusCode( 404 );
    return true;
}

bool HttpResponseSerializer::createHttpResponseForNotAllowed() {
    initializeHttpResponse();
    response_->setStatusCode( 405 );
    return true;
}


bool HttpResponseSerializer::cleanData() {
    // TODO: need to clean?
    return false;
}

void HttpResponseSerializer::serializeHttpResponse() {
    std::ostringstream serializedResponse;

    // Serializing request-line
    serializedResponse << "HTTP/" << std::to_string( response_->getVersionMajor() )
                        << "." << std::to_string( response_->getVersionMinor() )
                        << " " << getStatusCodeString( response_->getStatusCode() ) << MONZZA_HTTP_CRLF;

    // Serializing headers
    std::vector<HttpHeaderItem*>* headers_ = response_->getHeaders();
    for ( auto& headerItem : *headers_ ) {
        serializedResponse << headerItem->name << ": " << headerItem->value << MONZZA_HTTP_CRLF;
    }

    serializedResponse << MONZZA_HTTP_CRLF;

    buffer_->write( ( uint8_t* )serializedResponse.str().c_str(),
                    ( uint16_t )serializedResponse.str().length() );
}

uint32_t HttpResponseSerializer::getSerializedHttpResponseSize() {
    serializeHttpResponse();
    return buffer_->getWrittenDataSize();
}

bool HttpResponseSerializer::getSerializedHttpResponse( uint8_t* buf, uint32_t bufSize ) {
    return buffer_->read( buf, bufSize );
}

std::string HttpResponseSerializer::getContentTypeForFilePath( const std::string& fileExtention ) {
    if ( fileExtention == std::string( "html" ) ) {
        return MONZZA_HTTP_CONTENT_TYPE_HTML;
    }
    else if ( fileExtention == std::string( "txt") )  {
        return MONZZA_HTTP_CONTENT_TYPE_HTML;
    }
    else if ( fileExtention == std::string( "css" ) ) {
        return MONZZA_HTTP_CONTENT_TYPE_CSS;
    }
    else if ( fileExtention == std::string( "gif" ) ) {
        return MONZZA_HTTP_CONTENT_TYPE_GIF;
    }
    else if ( fileExtention == std::string( "jpeg" ) ) {
        return MONZZA_HTTP_CONTENT_TYPE_JPEG;
    }
    else if ( fileExtention == std::string( "jpg" ) ) {
        return MONZZA_HTTP_CONTENT_TYPE_JPEG;
    }
    else if ( fileExtention == std::string( "js" ) ) {
        return MONZZA_HTTP_CONTENT_TYPE_JS;
    }
    else if ( fileExtention == std::string( "png" ) ) {
        return MONZZA_HTTP_CONTENT_TYPE_PNG;
    }
    else if ( fileExtention == std::string( "swf" ) ) {
        return MONZZA_HTTP_CONTENT_TYPE_SWF;
    }
    return MONZZA_HTTP_CONTENT_TYPE_HTML;
}

std::string HttpResponseSerializer::getStatusCodeString( int16_t statusCode ) const {
    switch ( statusCode ) {
        case 200:
            return MONZZA_HTTP_RESPONSE_STATUS_200;
        case 403:
            return MONZZA_HTTP_RESPONSE_STATUS_403;
        case 404:
            return MONZZA_HTTP_RESPONSE_STATUS_404;
        case 405:
            return MONZZA_HTTP_RESPONSE_STATUS_405;
        default:
            return MONZZA_HTTP_RESPONSE_STATUS_500;
    }
}