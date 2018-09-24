#include "master_thread.hpp"

// =================================================================
// ========== MasterThreadServiceMessage class definition ==========
// =================================================================

bool MasterThreadServiceMessage::setResponseType( MasterThreadServiceMessage::ResponseType responseType ) {
    responseType_ = responseType;
    return true;
}

MasterThreadServiceMessage::ResponseType MasterThreadServiceMessage::getResponseType() const {
    return responseType_;
}

bool MasterThreadServiceMessage::setCommandType( MasterThreadServiceMessage::CommandType commandType ) {
    commandType_ = commandType;
    return true;
}

MasterThreadServiceMessage::CommandType MasterThreadServiceMessage::getCommandType() const {
    return commandType_;
}

// ===================================================
// ========== MasterThread class definition ==========
// ===================================================

MasterThread::MasterThread() {
    workerIdToUse_ = 0;
    table_ = new Table;
    breakThreadLoop_  = false;
    events_.push_back( inputServiceMessages_.getEventHandle() );
    events_.push_back( table_->getUpdatedInformationEvent_() );
}

MasterThread::~MasterThread() {
    delete table_;
}

bool MasterThread::initialize( Logger* logger, MasterSettings* masterSettings ) {
    if ( ( logger == nullptr ) || ( masterSettings == nullptr ) ) {
        return false;
    }

    if ( listenSocket_.isOpen() ) {
        listenSocket_.close();
    }

    masterSettings_ = masterSettings;
    bool openSocketResult = listenSocket_.open( masterSettings_->getIpAddress().getPortNumber(),
                                                false,
                                                masterSettings_->getMaxPendingEvents() );

    if ( openSocketResult && listenSocket_.isOpen() ) {
        if ( listenSocketEvent_.initializeEvent( listenSocket_, CPL_SOCKET_EVENT_TYPE_READ ) ) {
            events_.push_back( &listenSocketEvent_ );

            setLogger( logger );
            setModuleName( "MasterThread  " );

            return true;
        }
        else {
            listenSocket_.close();
            return false;
        }
    }
    else {
        masterSettings_ = nullptr;
        return false;
    }
}

cpl::Event* MasterThread::getNewOutputServiceMessageEvent() {
    return ( outputServiceMessages_.getEventHandle() );
}

void MasterThread::addInputServiceMessage( MasterThreadServiceMessage* masterThreadServiceMessage ) {
    inputServiceMessages_.push( masterThreadServiceMessage );
}

MasterThreadServiceMessage* MasterThread::getOutputServiceMessage() {
    MasterThreadServiceMessage* masterThreadServiceMessage;
    return ( outputServiceMessages_.tryPop( masterThreadServiceMessage ) ? masterThreadServiceMessage : nullptr );
}

void MasterThread::operator()() {
    uint32_t waitResult = 0;

    notificationMsg( "Module started." );

    notificationMsg( "Starting worker threads." );
    Worker* worker;
    WorkerSettings* workerSettings;
    bool workersStarted = true;
    for ( uint32_t i = 0; i < masterSettings_->getNumberOfWorkers(); i++ ) {
        workerSettings = new WorkerSettings;
        workerSettings->setId( i );
        worker = new Worker;
        if ( worker->start( getLogger(), table_, workerSettings ) ) {
            workers_[ i ] = std::pair<Worker*, WorkerSettings*>( worker, workerSettings );
        }
        else {
            workersStarted = false;
            break;
        }
    }

    sleep( 1 );

    if ( workersStarted ) {
        notificationMsg( "Worker threads started." );
    }
    else {
        notificationMsg( "Failed to start worker threads." );
        listenSocket_.close();
        listenSocketEvent_ = cpl::Event();
        for ( auto workerRecord : workers_ ) {
            workerRecord.second.first->stop();
            delete workerRecord.second.first;
            delete workerRecord.second.second;
        }
        workers_.clear();
        breakThreadLoop_ = true;
        events_.clear();
    }


    while ( !breakThreadLoop_ ) {
        waitResult = cpl::EventExpectant::waitForEvents( &events_, false, CPL_EE_WFE_INFINITE_WAIT );
        switch ( waitResult ) {
            case 0: // New service message.
                processInputServiceMessage();
                break;
            case 1: // Update workers information table.
                updateOptimalWorkerIdToUse();
                break;
            case 2: // New accept connection.
                notificationMsg( "Delegating new connection to worker." );
                processNewAcceptConnection();
                notificationMsg( "New connection delegated to worker." );
            default:
                break;
        }
    }

    notificationMsg( "Module stopped" );
}

void MasterThread::processInputServiceMessage() {
    MasterThreadServiceMessage* masterThreadServiceMessage;
    inputServiceMessages_.tryPop( masterThreadServiceMessage );
    serviceMsgHandler( masterThreadServiceMessage );
    outputServiceMessages_.push( masterThreadServiceMessage );
}

void MasterThread::serviceMsgHandler( MasterThreadServiceMessage* masterThreadServiceMessage ) {
    if ( masterThreadServiceMessage == nullptr ) {
        return;
    }

    switch ( masterThreadServiceMessage->getCommandType() ) {
        case MasterThreadServiceMessage::CommandType::STOP:
            stopServiceMsgHandler( masterThreadServiceMessage );
            break;
    }
}

void MasterThread::stopServiceMsgHandler( MasterThreadServiceMessage* masterThreadServiceMessage ) {
    masterThreadServiceMessage->setResponseType( MasterThreadServiceMessage::ResponseType::SUCCESSFUL );

    events_.clear();

    listenSocket_.close();
    listenSocketEvent_ = cpl::Event();

    notificationMsg( "Stopping worker threads." );
    for ( auto worker : workers_ ) {
        worker.second.first->stop();
        delete worker.second.first;
        delete worker.second.second;
    }
    sleep( 2 );
    notificationMsg( "Worker threads stopped." );

    workers_.clear();

    sleep( 2 );

    breakThreadLoop_ = true;
}

void MasterThread::processNewAcceptConnection() {
    cpl::TcpServerExchangeSocket* tcpServerExchangeSocket = listenSocket_.accept();
    if ( tcpServerExchangeSocket != nullptr ) {
        workers_[ workerIdToUse_ ].first->addNewConnection( tcpServerExchangeSocket );
        //if ( workerIdToUse_ == masterSettings_->getNumberOfWorkers() - 1 ) {
        //    workerIdToUse_ = 0;
        //}
        //else {
        //    workerIdToUse_++;
        //}
    }
}

void MasterThread::updateOptimalWorkerIdToUse() {
    notificationMsg( "Calculating optimal worker id to use." );

    table_->readWorkersInformation( workersInformation_ );

    uint32_t optimalWorkerId = 0;
    uint32_t minimalNumberOfConnections = 0xFFFF;

    for ( auto record : workersInformation_ ) {
        if ( record.second.first == 0 ) {
            optimalWorkerId = record.first;
            break;
        }
        else {
            if ( record.second.first < minimalNumberOfConnections ) {
                optimalWorkerId = record.first;
                minimalNumberOfConnections = record.second.first;
            }
        }
    }

    workerIdToUse_ = optimalWorkerId;

    notificationMsg( "Calculated optimal worker id to use: " + std::to_string( optimalWorkerId ) );
}