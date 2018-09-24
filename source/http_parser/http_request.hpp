#ifndef MONZZA_HTTP_REQUEST_HPP
#define MONZZA_HTTP_REQUEST_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace monzza {
    namespace core {
        namespace http {
            enum HttpRequestMethod {
                UNDEFINED,
                OPTIONS,
                GET,
                HEAD,
                POST,
                PUT,
                DELETE,
                TRACE,
                CONNECT
            };

            class HttpRequestHeaderItem {
            public:
                std::string name;
                std::string value;
            };

            class HttpRequest {
            public:
                HttpRequest();

                HttpRequestMethod getHttpMethod() const;
                const std::string &getUri() const;
                int8_t getVersionMajor() const;
                int8_t getVersionMinor() const;

                void setHttpMethod( HttpRequestMethod httpMethod );
                void setUri(const std::string &uri);
                void setVersionMajor( int8_t versionMajor );
                void setVersionMinor( int8_t versionMinor );

                void addHeaderItem( HttpRequestHeaderItem* headerItem );

            private:
                HttpRequestMethod httpMethod_;
                std::string uri_;
                int8_t versionMajor_;
                int8_t versionMinor_;
                std::vector<HttpRequestHeaderItem*> headers_;
                std::vector<uint8_t> content_;
                bool keepAlive_;
            };
        }
    }
}

#endif //MONZZA_HTTP_REQUEST_HPP
