#include "http_request.hpp"

using namespace monzza::http;

HttpRequest::HttpRequest() :
    httpMethod_( HttpRequestMethod::UNDEFINED ),
    versionMajor_( 0 ),
    versionMinor_( 0 ),
    keepAlive_( false )
{}

HttpRequest::~HttpRequest() {
    for ( auto header : headers_ ) {
        delete header;
    }
}


HttpRequestMethod HttpRequest::getHttpMethod() const {
    return httpMethod_;
}

void HttpRequest::setHttpMethod( HttpRequestMethod httpMethod ) {
    httpMethod_ = httpMethod;
}

const std::string& HttpRequest::getUri() const {
    return uri_;
}

void HttpRequest::setUri( const std::string& uri ) {
    uri_ = uri;
}

int8_t HttpRequest::getVersionMajor() const {
    return versionMajor_;
}

void HttpRequest::setVersionMajor( int8_t versionMajor ) {
    versionMajor_ = versionMajor;
}

int8_t HttpRequest::getVersionMinor() const {
    return versionMinor_;
}

void HttpRequest::setVersionMinor( int8_t versionMinor ) {
    versionMinor_ = versionMinor;
}

void HttpRequest::addHeaderItem( HttpHeaderItem* headerItem ) {
    headers_.push_back( headerItem );
}