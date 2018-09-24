#include "http_request.hpp"

using namespace monzza::core::http;

HttpRequest::HttpRequest() :
        httpMethod_( HttpRequestMethod::UNDEFINED ),
        versionMajor_(0),
        versionMinor_(0),
        keepAlive_( false ) {}

HttpRequestMethod HttpRequest::getHttpMethod() const {
    return httpMethod_;
}

void HttpRequest::setHttpMethod( HttpRequestMethod httpMethod ) {
    HttpRequest::httpMethod_ = httpMethod;
}

const std::string &HttpRequest::getUri() const {
    return uri_;
}

void HttpRequest::setUri( const std::string& uri ) {
    HttpRequest::uri_ = uri;
}

int8_t HttpRequest::getVersionMajor() const {
    return versionMajor_;
}

void HttpRequest::setVersionMajor( int8_t versionMajor ) {
    HttpRequest::versionMajor_ = versionMajor;
}

int8_t HttpRequest::getVersionMinor() const {
    return versionMinor_;
}

void HttpRequest::setVersionMinor( int8_t versionMinor ) {
    HttpRequest::versionMinor_ = versionMinor;
}

void HttpRequest::addHeaderItem( HttpRequestHeaderItem* headerItem ) {
    headers_.push_back( headerItem );
}
