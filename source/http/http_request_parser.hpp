#ifndef MONZZA_HTTP_PARSER_HPP
#define MONZZA_HTTP_PARSER_HPP

#include <cstdint>

#include "../logger/log_functionality.hpp"
#include "../buffer/buffer.hpp"

#include "http_request.hpp"

namespace monzza {
    namespace http {
        enum HttpRequestParserState {
            WAITING_FOR_HTTP_REQUEST_METHOD_START,
            WAITING_FOR_URI_START,
            WAITING_FOR_H,
            WAITING_FOR_HT,
            WAITING_FOR_HTT,
            WAITING_FOR_HTTP,
            WAITING_FOR_VERSION_SLASH,
            WAITING_FOR_VERSION_MAJOR,
            WAITING_FOR_VERSION_DOT_SEPARATION,
            WAITING_FOR_VERSION_MINOR,
            WAITING_FOR_REQUEST_LINE_CRLF,
            WAITING_FOR_REQUEST_HEADER_ITEM_START,
            WAITING_FOR_REQUEST_HEADER_ITEM_NAME,
            WAITING_FOR_REQUEST_HEADER_ITEM_VALUE,
            WAITING_FOR_REQUEST_HEADER_ITEM_CRLF,
            WAITING_FOR_END_REQUEST_CRLF
        };

        class HttpRequestParser : public monzza::logger::LogFunctionality {
        public:
            HttpRequestParser();
            ~HttpRequestParser();

            bool initialize( monzza::logger::Logger* logger, std::string parentModuleName );
            bool addDataAndParse( uint8_t* buf, uint32_t bufSize );
            bool cleanData();

            HttpRequest* getHttpRequest();

        protected:
            HttpParsingResult parse();
            HttpParsingResult tryToParseHttpRequestMethod();
            HttpParsingResult tryToParseUri();
            HttpParsingResult tryToParseRequestHeaderName( HttpHeaderItem* item );
            HttpParsingResult tryToParseRequestHeaderValue( HttpHeaderItem* item );

            std::string urlDecode( std::string encodedUrl );
            std::string getPathFromUrl( const std::string &url );

            bool setState( HttpRequestParserState state );

            uint32_t skipBytesUntilSymbol( char c );

            bool isSymbolChar( uint8_t bufChar );
            bool isControlChar( uint8_t bufChar );
            bool isSpecialChar( uint8_t bufChar );
            bool isDigitChar( uint8_t bufChar );

        private:
            monzza::buffer::Buffer* buffer_;
            HttpRequest* request_;
            HttpRequestParserState state_;
        };
    }
}

#endif //MONZZA_HTTP_PARSER_HPP
