#include "http_response.hpp"

#include <iostream>

using namespace monzza::http;

HttpResponse::HttpResponse() :
        versionMajor_( 1 ),
        versionMinor_( 1 )
{}

HttpResponse::~HttpResponse() {
    for ( auto header : headers_ ) {
        delete header;
    }
}

int8_t HttpResponse::getVersionMajor() const {
    return versionMajor_;
}

void HttpResponse::setVersionMajor( int8_t versionMajor ) {
    versionMajor_ = versionMajor;
}

int8_t HttpResponse::getVersionMinor() const {
    return versionMinor_;
}

void HttpResponse::setVersionMinor( int8_t versionMinor ) {
    versionMinor_ = versionMinor;
}

int16_t HttpResponse::getStatusCode() const {
    return statusCode_;
}

void HttpResponse::setStatusCode( int16_t statusCode ) {
    statusCode_ = statusCode;
}

std::vector<HttpHeaderItem*>* HttpResponse::getHeaders() {
    return &headers_;
}

void HttpResponse::addHeaderItem( std::string name, std::string value ) {
    auto headerItem = new HttpHeaderItem();
    headerItem->name = std::move( name );
    headerItem->value = std::move( value );
    headers_.push_back( headerItem );
}