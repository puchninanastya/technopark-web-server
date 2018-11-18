#ifndef MONZZA_HTTP_FILE_SENDER_HPP
#define MONZZA_HTTP_FILE_SENDER_HPP

#include "../logger/log_functionality.hpp"

namespace monzza {
    namespace http {
        static std::vector<std::string> SupportedExtensions = { ".html",
                                                                ".css",
                                                                ".js",
                                                                ".jpg",
                                                                ".jpeg",
                                                                ".png",
                                                                ".gif",
                                                                ".swf",
                                                                ".txt" };

        enum class HttpFileReachability {
            EXISTS,
            NOT_EXISTS,
            ACCESS_DENIED
        };

        class HttpFileDescription {
        public:
            HttpFileDescription() {
                fileSize = 0;
                offset = 0;
            }

            HttpFileReachability httpFileReachability;
            std::string fileName;
            std::string filePath;
            std::string fileType;
            uint32_t fileSize;
            off_t offset;
        };

        class HttpFileSender : public monzza::logger::LogFunctionality {
        public:
            bool initialize( monzza::logger::Logger* logger,
                             std::string parentModuleName,
                             std::string documentRoot );

            HttpFileDescription getFileDescription( std::string relativePath );

            bool sendFileThroughSocket( cpl::TcpServerExchangeSocket* tcpServerExchangeSocket,
                                        HttpFileDescription& httpFileDescription );
        private:
            std::string documentRoot_;
        };
    }
}

#endif // MONZZA_HTTP_FILE_SENDER_HPP