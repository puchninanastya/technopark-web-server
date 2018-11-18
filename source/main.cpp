#include <iostream>
#include <csignal>

#include "settings/settings.hpp"
#include "master/master.hpp"

#define MONZZA_SERVER_PORT              80
#define MONZZA_SERVER_CONFIG_FILE       "/etc/httpd.conf"

static monzza::master::Master*          master = nullptr;
static monzza::master::MasterSettings*  masterSettings = nullptr;
static monzza::settings::Settings*      monzzaCoreSettings = nullptr;
static monzza::logger::Logger*          logger = nullptr;
static cpl::IpAddress*                  serverIpAddress = nullptr;

void cleanUpBeforeExit() {
    std::cout << "Cleaning up before exit.." << std::endl;
    if ( master != nullptr) {
        master->stop();
        sleep( 2 );
    }
    if ( logger != nullptr ) {
        logger->stop();
    }
    delete master;
    delete logger;
    delete serverIpAddress;
    delete masterSettings;
    delete monzzaCoreSettings;
    std::cout << "Cleaning up ended." << std::endl;
}

void exitHandler( int signal, siginfo_t* siginfo, void* context ) {
    std::cout << "Signal Number = " << siginfo->si_signo << std::endl;
    std::cout << "Signal Code = " << siginfo->si_code << std::endl;
    std::cout << "Sending PID = " << ( long )siginfo->si_pid << std::endl;
    std::cout << "Sending User ID = " << ( long )siginfo->si_uid << std::endl;
    cleanUpBeforeExit();
    cpl::CplBase::shutdown();
    exit(0);
}

bool setHandlerToSignals() {
    struct sigaction sa;
    memset( &sa, 0, sizeof( sa ) );

    sa.sa_sigaction = exitHandler;
    sa.sa_flags = SA_SIGINFO;

    if ( sigemptyset( &sa.sa_mask ) != 0 ) {
        return false;
    }

    if ( sigaddset( &sa.sa_mask, SIGTERM ) != 0 ) {
        return false;
    }
    if ( sigaddset( &sa.sa_mask, SIGHUP ) != 0 ) {
        return false;
    }
    if ( sigaddset( &sa.sa_mask, SIGQUIT ) != 0 ) {
        return false;
    }
    if ( sigaddset( &sa.sa_mask, SIGINT ) != 0 ) {
        return false;
    }

    sigaction( SIGTERM, &sa, 0 );
    sigaction( SIGHUP, &sa, 0 );
    sigaction( SIGQUIT, &sa, 0 );
    sigaction( SIGINT, &sa, 0 );

    return true;
}

void application() {
    logger = new monzza::logger::Logger();

    if ( !logger->start() ) {
        std::cout << "Error: Failed to start Logger." << std::endl;
        delete logger;
        return;
    }

    monzzaCoreSettings = new monzza::settings::Settings();

    if ( !monzzaCoreSettings->readSettingsFromConfigFile( MONZZA_SERVER_CONFIG_FILE ) ) {
        std::cout << "Error: Failed to read settings from httpd.conf." << std::endl;
        logger->stop();
        delete logger;
        return;
    }

    serverIpAddress = new cpl::IpAddress;
    serverIpAddress->setPortNumber( MONZZA_SERVER_PORT );

    masterSettings = new monzza::master::MasterSettings;
    masterSettings->setIpAddress( *serverIpAddress );
    masterSettings->setMaxPendingEvents( 5000 );
    masterSettings->setNumberOfWorkers( monzzaCoreSettings->getThreadLimit() );
    masterSettings->setDocumentRoot( monzzaCoreSettings->getDocumentRoot() );

    master = new monzza::master::Master();

    if ( master->start( logger, masterSettings ) ) {
        std::cout << std::endl << "Type any key to stop HTTP server." << std::endl << std::endl;

        // getchar();

        while ( true ) {
            sleep(86400);
        }

    } else {
        std::cout << "Failed to start HTTP server." << std::endl;
        cleanUpBeforeExit();
    }
}

int main() {
    if ( !cpl::CplBase::initialize() ) {
        std::cout << "Failed to initialize CPL library." << std::endl;
        return 1;
    }
    else {
        setHandlerToSignals();
        application();

        while ( true ) {
            sleep(86400);
        }

        return 0;
    }
}