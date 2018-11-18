#ifndef MONZZA_HTTP_RESPONSE_SERIALIZER_HPP
#define MONZZA_HTTP_RESPONSE_SERIALIZER_HPP

#include <cstdint>
#include <string.h>

#include "../logger/log_functionality.hpp"
#include "../buffer/buffer.hpp"

#include "http_response.hpp"
#include "http_file_sender.hpp"

namespace monzza {
    namespace http {
        class HttpResponseSerializer : public monzza::logger::LogFunctionality {
        public:
            HttpResponseSerializer();
            ~HttpResponseSerializer();

            bool initialize( monzza::logger::Logger* logger, std::string parentModuleName );
            void initializeHttpResponse();

            bool createHttpResponseForExistingFile( HttpFileDescription httpFileDescription );
            bool createHttpResponseForNotFound();
            bool createHttpResponseForForbidden();
            bool createHttpResponseForNotAllowed();

            uint32_t getSerializedHttpResponseSize();
            bool getSerializedHttpResponse( uint8_t* buf, uint32_t bufSize );

            bool cleanData();

        protected:
            void serializeHttpResponse();

            std::string getContentTypeForFilePath( const std::string& fileExtension );
            std::string getStatusCodeString( int16_t statusCode ) const;

        private:
            monzza::buffer::Buffer* buffer_;
            HttpResponse* response_;
        };
    }
}

#endif // MONZZA_HTTP_RESPONSE_SERIALIZER_HPP
