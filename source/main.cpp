#include <iostream>
#include <csignal>

#include "settings/core_settings.hpp"
#include "http_parser/http_parser.hpp"
#include "master/master.hpp"

void exitHandler( int signal, siginfo_t* siginfo, void* context ) {
    std::cout << "Signal Number = " << siginfo->si_signo << std::endl;
    std::cout << "Signal Code = " << siginfo->si_code << std::endl;
    std::cout << "Sending PID = " << ( long )siginfo->si_pid << std::endl;
    std::cout << "Sending User ID = " << ( long )siginfo->si_uid << std::endl;
}

bool setHandlerToSignals() {
    struct sigaction sa;
    memset( &sa, 0, sizeof( sa ) );

    sa.sa_sigaction = exitHandler;
    sa.sa_flags = SA_SIGINFO;

    if ( sigemptyset( &sa.sa_mask ) != 0 )
        return false;

    if ( sigaddset( &sa.sa_mask, SIGTERM ) != 0 )
        return false;
    if ( sigaddset( &sa.sa_mask, SIGHUP ) != 0 )
        return false;
    if ( sigaddset( &sa.sa_mask, SIGQUIT ) != 0 )
        return false;
    if ( sigaddset( &sa.sa_mask, SIGINT ) != 0 )
        return false;

    sigaction( SIGTERM, &sa, 0 );
    sigaction( SIGHUP, &sa, 0 );
    sigaction( SIGQUIT, &sa, 0 );
    sigaction( SIGINT, &sa, 0 );

    return true;
}

void application() {
    auto logger = new Logger();
    if ( !logger->start() ) {
        std::cout << "Error: Failed to start Logger." << std::endl;
        delete logger;
        return;
    }

    // test settings
    auto monzzaCoreSettings = new monzza::CoreSettings();
    if ( !monzzaCoreSettings->readSettingsFromConfigFile( "httpd.conf" ) ) {
        return;
    }

    // test http parser
    uint8_t inputBuffer[] = "GET /index.php HTTP/1.1\r\nHost: example.com\r\nAccept: text/html\r\n\r\n";
    auto httpParser = new monzza::http::HttpParser();

    if ( !httpParser->addData( inputBuffer, sizeof inputBuffer ) ) {
        return;
    }

    // test echo tcp server with thread pool
    auto ipAddress = new cpl::IpAddress;
    ipAddress->setPortNumber( monzzaCoreSettings->getListeningPort() );

    auto masterSettings = new MasterSettings;
    masterSettings->setIpAddress( *ipAddress );
    masterSettings->setMaxPendingEvents( 5000 );
    masterSettings->setNumberOfWorkers( monzzaCoreSettings->getCpuLimit() );

    auto master = new Master();

    if ( master->start( logger, masterSettings ) ) {
        std::cout << "Type any key to stop HTTP server." << std::endl << std::endl;
        getchar();
        master->stop();
        sleep( 2 );
        logger->stop();
        delete master;
        delete logger;
        delete ipAddress;
        delete masterSettings;
        delete monzzaCoreSettings;
    } else {
        std::cout << "Failed to start HTTP server." << std::endl;
        delete master;
        logger->stop();
        delete logger;
        delete ipAddress;
        delete masterSettings;
        delete monzzaCoreSettings;
    }
}

int main() {
    if ( !cpl::CplBase::initialize() ) {
        std::cout << "Failed to initialize CPL library." << std::endl;
        getchar();
        return 1;
    } else {
        application();
        getchar();
        cpl::CplBase::close();
        return 0;
    }
}