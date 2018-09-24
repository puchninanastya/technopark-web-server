#include "master.hpp"

Master::Master() {
    thread_ = nullptr;
    masterThread_ = nullptr;
}

bool Master::start( Logger* logger, MasterSettings* masterSettings ) {
    if ( ( logger == nullptr ) || ( masterSettings == nullptr ) ) {
        return false;
    }

    if ( ( thread_ != nullptr ) || ( masterThread_ != nullptr ) ) {
        return false;
    }

    setLogger( logger );

    masterThread_ = new MasterThread();

    if ( !masterThread_->initialize( getLogger(), masterSettings ) ) {
        delete masterThread_;
        masterThread_ = nullptr;
        setLogger( nullptr );
        return false;
    }

    thread_ = new std::thread( std::ref( *( masterThread_ ) ) );

    return true;
}

bool Master::stop() {
    if ( ( thread_ == nullptr ) || ( masterThread_ == nullptr ) ) {
        return true;
    }

    auto masterThreadServiceMessage = new MasterThreadServiceMessage;
    masterThreadServiceMessage->setCommandType( MasterThreadServiceMessage::CommandType::STOP );

    masterThread_->addInputServiceMessage( masterThreadServiceMessage );
    cpl::Event* outputServiceMessageEvent = masterThread_->getNewOutputServiceMessageEvent();
    cpl::EventExpectant::waitForEvent( outputServiceMessageEvent, CPL_EE_WFE_INFINITE_WAIT );
    masterThreadServiceMessage = masterThread_->getOutputServiceMessage();
    MasterThreadServiceMessage::ResponseType responseType = masterThreadServiceMessage->getResponseType();

    delete masterThreadServiceMessage;

    if ( responseType == MasterThreadServiceMessage::ResponseType::SUCCESSFUL ) {
        thread_->join();
        delete thread_;
        thread_ = nullptr;
        delete masterThread_;
        masterThread_ = nullptr;
        return true;
    }
    else {
        return false;
    }
}
