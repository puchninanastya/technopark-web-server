#ifndef MONZZA_HTTP_PARSER_HPP
#define MONZZA_HTTP_PARSER_HPP

#include <cstdint>

#include "http_request.hpp"
#include "buffer.hpp"

namespace monzza {
    namespace http {

        enum HttpParserState {
            WaitingForHttpRequestMethodStart,
            WaitingForUriStart,
            WaitingForH,
            WaitingForHT,
            WaitingForHTT,
            WaitingForHTTP,
            WaitingForVersionSlash,
            WaitingForVersionMajor,
            WaitingForVersionDotSeparation,
            WaitingForVersionMinor,
            WaitingForRequestLineCRLF,
            WaitingForRequestHeaderItemStart,
            WaitingForRequestHeaderItemName,
            WaitingForRequestHeaderItemValue,
            WaitingForRequestHeaderItemCRLF,
            WaitingForEndRequestCRLF
        };

        enum HttpParsingResult {
            Completed,
            NeedMoreData,
            ParsingError
        };

        class HttpParser {
        public:
            HttpParser();
            ~HttpParser();

            bool addData( uint8_t* buf, uint32_t bufSize );

        protected:
            HttpParsingResult parse();
            HttpParsingResult tryToParseHttpRequestMethod();
            HttpParsingResult tryToParseUri();
            HttpParsingResult tryToParseRequestHeaderName( HttpRequestHeaderItem* item );
            HttpParsingResult tryToParseRequestHeaderValue( HttpRequestHeaderItem* item );

            bool setState( HttpParserState state );

            uint32_t skipBytesUntilSymbol( char c );

            bool isSymbolChar( uint8_t bufChar );
            bool isControlChar( uint8_t bufChar );
            bool isSpecialChar( uint8_t bufChar );
            bool isDigitChar( uint8_t bufChar );

        private:
            Buffer* buffer_;
            HttpRequest* request_;
            HttpParserState state_;
        };


    }
}

#endif //MONZZA_HTTP_PARSER_HPP
