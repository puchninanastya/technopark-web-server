#include <experimental/filesystem>
#include <sys/sendfile.h>
#include <string>

#include "http_file_sender.hpp"

using namespace monzza::http;
namespace fs = std::experimental::filesystem;

bool HttpFileSender::initialize( monzza::logger::Logger* logger ,
                                 std::string parentModuleName,
                                 std::string documentRoot )
{
    if ( logger == nullptr ) {
        return false;
    }

    setLogger( logger );
    setModuleName( parentModuleName );

    documentRoot_ = std::move( documentRoot );

    return true;
}

HttpFileDescription HttpFileSender::getFileDescription( std::string requestPath ) {
    HttpFileDescription httpFileDescription;

    std::error_code errorCode;
    fs::path documentRootAbsolutePath( documentRoot_.c_str() );

    std::vector<std::string> documentRootAbsolutePathFolders;
    for ( const auto& part : documentRootAbsolutePath ) {
        documentRootAbsolutePathFolders.push_back( part.string() );
    }

    fs::path requestRelativePath( requestPath.c_str() );
    fs::path requestAbsolutePath;
    requestAbsolutePath = fs::canonical( documentRootAbsolutePath / requestRelativePath, errorCode );

    if ( requestAbsolutePath.empty() ) {
        httpFileDescription.httpFileReachability = HttpFileReachability::NOT_EXISTS;
        return httpFileDescription;
    }

    std::vector<std::string> requestAbsolutePathFolders;
    for ( const auto& part : requestAbsolutePath ) {
        requestAbsolutePathFolders.push_back( part.string() );
    }

    if ( requestAbsolutePathFolders.size() < documentRootAbsolutePathFolders.size() ) {
        httpFileDescription.httpFileReachability = HttpFileReachability::ACCESS_DENIED;
        return httpFileDescription;
    }
    else {
        for ( uint8_t i = 0; i < documentRootAbsolutePathFolders.size(); i++ ) {
            if ( documentRootAbsolutePathFolders[ i ] != requestAbsolutePathFolders[ i ] ) {
                httpFileDescription.httpFileReachability = HttpFileReachability::ACCESS_DENIED;
                return httpFileDescription;
            }
        }
    }

    if ( fs::exists( requestAbsolutePath )) {
        if ( fs::is_directory( requestAbsolutePath )) {
            requestAbsolutePath.append( "index.html" );

            if ( fs::exists( requestAbsolutePath ) ) {
                httpFileDescription.fileSize = fs::file_size( requestAbsolutePath );
                httpFileDescription.fileType = requestAbsolutePath.extension();

                httpFileDescription.httpFileReachability = HttpFileReachability::EXISTS;
                httpFileDescription.fileName = requestAbsolutePath.filename().string();
                httpFileDescription.filePath = requestAbsolutePath.string();
            }
            else {
                httpFileDescription.httpFileReachability = HttpFileReachability::ACCESS_DENIED;
            }
        }
        else {
            httpFileDescription.fileSize = fs::file_size( requestAbsolutePath );
            httpFileDescription.fileType = requestAbsolutePath.extension();

            httpFileDescription.httpFileReachability = HttpFileReachability::ACCESS_DENIED;
            for ( uintmax_t i = 0; i < SupportedExtensions.size(); i++) {
                if ( httpFileDescription.fileType == SupportedExtensions[ i ] ) {
                    httpFileDescription.fileType.erase( 0, 1 );
                    httpFileDescription.httpFileReachability = HttpFileReachability::EXISTS;
                    httpFileDescription.fileName = requestAbsolutePath.filename().string();
                    httpFileDescription.filePath = requestAbsolutePath.string();
                    break;
                }
            }

        }
    }

    return httpFileDescription;
}

bool HttpFileSender::sendFileThroughSocket( cpl::TcpServerExchangeSocket* tcpServerExchangeSocket,
                                            HttpFileDescription& httpFileDescription )
{
    if ( !tcpServerExchangeSocket->isOpen() ) {
        return false;
    }

    int fileDescriptor = open( httpFileDescription.filePath.c_str(), O_RDONLY );

    if ( fileDescriptor < 0 ) {
        return false;
    }
    else {
        sendfile( tcpServerExchangeSocket->getPlatformSocket(),
                  fileDescriptor,
                  &httpFileDescription.offset,
                  httpFileDescription.fileSize - ( uint32_t )httpFileDescription.offset );

        if ( httpFileDescription.offset >= httpFileDescription.fileSize ) {
                httpFileDescription.offset = 0;
        }

        close( fileDescriptor );

        return true;
    }
}