#include "worker_thread.hpp"

// =================================================================
// ========== WorkerThreadServiceMessage class definition ==========
// =================================================================

bool WorkerThreadServiceMessage::setResponseType( WorkerThreadServiceMessage::ResponseType responseType ) {
    responseType_ = responseType;
    return true;
}

WorkerThreadServiceMessage::ResponseType WorkerThreadServiceMessage::getResponseType() const {
    return responseType_;
}

bool WorkerThreadServiceMessage::setCommandType( WorkerThreadServiceMessage::CommandType commandType ) {
    commandType_ = commandType;
    return true;
}

WorkerThreadServiceMessage::CommandType WorkerThreadServiceMessage::getCommandType() const {
    return commandType_;
}

bool WorkerThreadServiceMessage::setNewConnection( cpl::TcpServerExchangeSocket* tcpServerExchangeSocket ) {
    tcpServerExchangeSocket_ = tcpServerExchangeSocket;
    return true;
}

cpl::TcpServerExchangeSocket* WorkerThreadServiceMessage::getNewConnection() const {
    return tcpServerExchangeSocket_;
}

// ===================================================
// ========== WorkerThread class definition ==========
// ===================================================

WorkerThread::WorkerThread() {
    breakThreadLoop_  = false;
    bufPtr_ = new uint8_t[ 8192 ];
    bufSize_ = 8192;
    httpParser_ = new HttpParser();
    events_.push_back( inputServiceMessages_.getEventHandle() );
}

WorkerThread::~WorkerThread() {
    delete[] bufPtr_;
    delete httpParser_;
}

bool WorkerThread::initialize( Logger* logger, Table* table, WorkerSettings* workerSettings ) {
    if ( ( logger == nullptr ) || ( table == nullptr ) || ( workerSettings == nullptr ) ) {
        return false;
    }

    setLogger( logger );
    std::string moduleName( "WorkerThread " );
    moduleName += std::to_string( workerSettings->getId() );
    setModuleName( moduleName );

    table_ = table;
    workerSettings_ = workerSettings;

    httpParser_->initialize( getLogger() );

    return true;
}

cpl::Event* WorkerThread::getNewOutputServiceMessageEvent() {
    return ( outputServiceMessages_.getEventHandle() );
}

void WorkerThread::addInputServiceMessage( WorkerThreadServiceMessage* workerThreadServiceMessage ) {
    inputServiceMessages_.push( workerThreadServiceMessage );
}

WorkerThreadServiceMessage* WorkerThread::getOutputServiceMessage() {
    WorkerThreadServiceMessage* workerThreadServiceMessage;
    return ( outputServiceMessages_.tryPop( workerThreadServiceMessage ) ? workerThreadServiceMessage : nullptr );
}

void WorkerThread::operator()() {
    uint32_t waitResult = 0;

    notificationMsg( "Module started." );

    notificationMsg( "Updating information about worker." );
    table_->updateWorkerInformation( workerSettings_->getId(), ( uint32_t )tcpServerExchangeSockets_.size(), 0  );
    notificationMsg( "Updated information about worker." );

    while ( !breakThreadLoop_ ) {
        waitResult = cpl::EventExpectant::waitForEvents( &events_, false, CPL_EE_WFE_INFINITE_WAIT );
        switch ( waitResult ) {
            case 0:  // New service message.
                processInputServiceMessage();
                break;
            default: // Tcp server exchange socket event.
                if ( waitResult > 0 ) {
                    processTcpServerExchangeSocketEvent( waitResult );
                }
                break;
        }
    }

    notificationMsg( "Module stopped." );
}

void WorkerThread::processInputServiceMessage() {
    WorkerThreadServiceMessage* workerThreadServiceMessage;
    inputServiceMessages_.tryPop( workerThreadServiceMessage );
    serviceMsgHandler( workerThreadServiceMessage );
    outputServiceMessages_.push( workerThreadServiceMessage );
}

void WorkerThread::serviceMsgHandler( WorkerThreadServiceMessage* workerThreadServiceMessage ) {
    if ( workerThreadServiceMessage == nullptr ) {
        return;
    }

    switch ( workerThreadServiceMessage->getCommandType() ) {
        case WorkerThreadServiceMessage::CommandType ::NEW_CONNECTION:
            newConnectionServiceMsgHandler( workerThreadServiceMessage );
            break;
        case WorkerThreadServiceMessage::CommandType::STOP:
            stopServiceMsgHandler( workerThreadServiceMessage );
            break;
    }
}

void WorkerThread::newConnectionServiceMsgHandler( WorkerThreadServiceMessage* workerThreadServiceMessage ) {
    notificationMsg( "Adding new connection." );
    cpl::TcpServerExchangeSocket* tcpServerExchangeSocket = workerThreadServiceMessage->getNewConnection();
    auto tcpServerExchangeSocketEvent = new cpl::Event;
    tcpServerExchangeSocketEvent->initializeEvent( *( tcpServerExchangeSocket ),  CPL_SOCKET_EVENT_TYPE_READ );
    std::pair<cpl::TcpServerExchangeSocket*, cpl::Event*> pair( tcpServerExchangeSocket,
                                                                tcpServerExchangeSocketEvent );
    tcpServerExchangeSockets_.push_back( pair );
    events_.push_back( tcpServerExchangeSocketEvent );
    notificationMsg( "Added new connection." );
    notificationMsg( "Updating information about worker." );
    table_->updateWorkerInformation( workerSettings_->getId(), ( uint32_t )tcpServerExchangeSockets_.size(), 0  );
    notificationMsg( "Updated information about worker." );
}

void WorkerThread::stopServiceMsgHandler( WorkerThreadServiceMessage* workerThreadServiceMessage ) {
    workerThreadServiceMessage->setResponseType( WorkerThreadServiceMessage::ResponseType::SUCCESSFUL );
    events_.clear();
    for ( uint32_t i = 0; i < tcpServerExchangeSockets_.size(); i++ ) {
        tcpServerExchangeSockets_[ i ].first->close();
        delete tcpServerExchangeSockets_[ i ].second;
        delete tcpServerExchangeSockets_[ i ].first;
    }
    breakThreadLoop_ = true;
}

void WorkerThread::processTcpServerExchangeSocketEvent( uint32_t index ) {
    notificationMsg("Process event with index ");
    cpl::TcpServerExchangeSocket* tcpServerExchangeSocket = ( tcpServerExchangeSockets_[ ( index - 1 ) ] ).first;
    int32_t result = tcpServerExchangeSocket->receive( bufPtr_, bufSize_ );
    if ( result > 0 ) {
        notificationMsg( "Processing connection request." );
        httpParser_->addData( bufPtr_, ( uint16_t )result );
        // tcpServerExchangeSocket->send( bufPtr_, ( uint16_t )result );
        notificationMsg( "Processed connection request." );
    } else {
        notificationMsg( "Deleting connection." );
        delete tcpServerExchangeSockets_[ index - 1 ].first;
        delete tcpServerExchangeSockets_[ index - 1 ].second;
        tcpServerExchangeSockets_.erase( tcpServerExchangeSockets_.begin() + ( index - 1 ) );
        events_.erase( events_.begin() + index );
        notificationMsg( "Connection deleted." );

        notificationMsg( "Updating information about worker." );
        table_->updateWorkerInformation( workerSettings_->getId(), ( uint32_t )tcpServerExchangeSockets_.size(), 0  );
        notificationMsg( "Updated information about worker." );
    }
};