#include "worker.hpp"

Worker::Worker() {
    thread_ = nullptr;
    workerThread_ = nullptr;
}

bool Worker::start( Logger* logger, Table* table, WorkerSettings* workerSettings ) {
    if ( ( logger == nullptr ) || ( table == nullptr ) || ( workerSettings == nullptr ) ) {
        return false;
    }

    if ( ( thread_ != nullptr ) || ( workerThread_ != nullptr ) ) {
        return false;
    }

    setLogger( logger );

    workerThread_ = new WorkerThread();

    if ( !workerThread_->initialize( getLogger(), table, workerSettings ) ) {
        delete workerThread_;
        workerThread_ = nullptr;
        return false;
    }

    setLogger( logger );

    thread_ = new std::thread( std::ref( *( workerThread_ ) ) );

    return true;
}

bool Worker::addNewConnection( cpl::TcpServerExchangeSocket* tcpServerExchangeSocket ) {
    if ( ( thread_ == nullptr ) || ( workerThread_ == nullptr ) ) {
        return false;
    }

    auto workerThreadServiceMessage = new WorkerThreadServiceMessage;
    workerThreadServiceMessage->setCommandType( WorkerThreadServiceMessage::CommandType::NEW_CONNECTION );
    workerThreadServiceMessage->setNewConnection( tcpServerExchangeSocket );
    workerThread_->addInputServiceMessage( workerThreadServiceMessage );

    return true;
}

bool Worker::stop() {
    if ( ( thread_ == nullptr ) || ( workerThread_ == nullptr ) ) {
        return true;
    }

    auto workerThreadServiceMessage = new WorkerThreadServiceMessage;
    workerThreadServiceMessage->setCommandType( WorkerThreadServiceMessage::CommandType::STOP );

    workerThread_->addInputServiceMessage( workerThreadServiceMessage );
    cpl::Event* outputServiceMessageEvent = workerThread_->getNewOutputServiceMessageEvent();
    cpl::EventExpectant::waitForEvent( outputServiceMessageEvent, CPL_EE_WFE_INFINITE_WAIT );
    workerThreadServiceMessage = workerThread_->getOutputServiceMessage();
    WorkerThreadServiceMessage::ResponseType responseType = workerThreadServiceMessage->getResponseType();

    delete workerThreadServiceMessage;

    if ( responseType == WorkerThreadServiceMessage::ResponseType::SUCCESSFUL ) {
        thread_->join();
        delete thread_;
        thread_ = nullptr;
        delete workerThread_;
        workerThread_ = nullptr;
        return true;
    }
    else {
        return false;
    }
}