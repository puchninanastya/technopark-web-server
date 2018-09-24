#include "table.hpp"

Table::Table() {
    updatedInformationEvent_ = new cpl::Event;
    updatedInformationEvent_->initializeEvent();
}

Table::~Table() {
    std::lock_guard<std::mutex> lock( m_ );
    delete updatedInformationEvent_;
}

bool Table::updateWorkerInformation( uint32_t workerId, uint32_t numberOfActiveConnections, uint32_t outputSpeed ) {
    std::lock_guard<std::mutex> lock( m_ );
    workersInformation_[ workerId ] = std::pair<uint32_t , uint32_t>( numberOfActiveConnections, outputSpeed );

    if ( !updatedInformationEvent_->isSignaled() ) {
        updatedInformationEvent_->setEvent();
    }

    return true;
}

void Table::readWorkersInformation( std::map <uint32_t, std::pair<uint32_t, uint32_t>>& workersInformation ) {
    std::lock_guard<std::mutex> lock( m_ );
    workersInformation.clear();
    workersInformation =  workersInformation_;
    //workersInformation_.clear();
    updatedInformationEvent_->resetEvent();
}

void Table::clear() {
    std::lock_guard<std::mutex> lock( m_ );
    workersInformation_.clear();
    updatedInformationEvent_->resetEvent();
}

cpl::Event* Table::getUpdatedInformationEvent_() const {
    return updatedInformationEvent_;
}