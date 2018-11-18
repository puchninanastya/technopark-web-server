#ifndef MONZZA_HTTP_HELPERS_HPP
#define MONZZA_HTTP_HELPERS_HPP

#include <string>

#define MONZZA_HTTP_RESPONSE_STATUS_200     "200 Ok"
#define MONZZA_HTTP_RESPONSE_STATUS_403     "403 Forbidden"
#define MONZZA_HTTP_RESPONSE_STATUS_404     "404 Not Found"
#define MONZZA_HTTP_RESPONSE_STATUS_405     "405 Not Allowed"
#define MONZZA_HTTP_RESPONSE_STATUS_500     "500 Internal error"

#define MONZZA_HTTP_HEADER_SERVER           "Server"
#define MONZZA_HTTP_HEADER_CONNECTION       "Connection"
#define MONZZA_HTTP_HEADER_DATE             "Date"
#define MONZZA_HTTP_HEADER_CONTENT_LENGTH   "Content-Length"
#define MONZZA_HTTP_HEADER_CONTENT_TYPE     "Content-Type"

#define MONZZA_HTTP_CONTENT_TYPE_HTML       "text/html"
#define MONZZA_HTTP_CONTENT_TYPE_CSS        "text/css"
#define MONZZA_HTTP_CONTENT_TYPE_GIF        "image/gif"
#define MONZZA_HTTP_CONTENT_TYPE_JPEG       "image/jpeg"
#define MONZZA_HTTP_CONTENT_TYPE_JS         "application/javascript"
#define MONZZA_HTTP_CONTENT_TYPE_PNG        "image/png"
#define MONZZA_HTTP_CONTENT_TYPE_SWF        "application/x-shockwave-flash"

#define MONZZA_HTTP_CRLF                    "\r\n"

namespace monzza {
    namespace http {
        class HttpHeaderItem {
        public:
            std::string name;
            std::string value;
        };

        enum class HttpRequestMethod {
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

        enum class HttpParsingResult {
            COMPLETED,
            NEED_MORE_DATA,
            PARSING_ERROR
        };

        enum class HttpSerializingResult {
            COMPLETED,
            SERIALIZING_ERROR
        };
    }
}

#endif // MONZZA_HTTP_HELPERS_HPP
