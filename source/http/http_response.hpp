#ifndef MONZZA_HTTP_RESPONSE_HPP
#define MONZZA_HTTP_RESPONSE_HPP

#include <string>
#include <vector>

#include "http_helpers.hpp"
#include "http_request.hpp"

namespace monzza {
    namespace http {

        class HttpResponse {
        public:
            HttpResponse();
            ~HttpResponse();

            int8_t getVersionMajor() const;
            int8_t getVersionMinor() const;
            int16_t getStatusCode() const;
            std::vector<HttpHeaderItem*>* getHeaders();

            void setVersionMajor( int8_t versionMajor );
            void setVersionMinor( int8_t versionMinor );
            void setStatusCode( int16_t statusCode );
            void addHeaderItem( std::string name, std::string value );

        private:
            int8_t versionMajor_;
            int8_t versionMinor_;
            int16_t statusCode_;
            std::vector<HttpHeaderItem*> headers_;
            std::vector<uint8_t> content_;
        };
    }
}

#endif // MONZZA_HTTP_RESPONSE_HPP
