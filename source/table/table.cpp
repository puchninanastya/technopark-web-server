#include "table.hpp"

using namespace monzza::table;

using WorkerInformation = std::pair<uint32_t , uint32_t>;

Table::Table() {
    updatedInformationEvent_ = new cpl::Event;
    updatedInformationEvent_->initializeEvent();
}

Table::~Table() {
    std::lock_guard<std::mutex> lock( m_ );
    delete updatedInformationEvent_;
}

bool Table::updateWorkerInformation( uint32_t workerId,
                                     uint32_t numberOfActiveConnections,
                                     uint32_t outputSpeed )
{
    std::lock_guard<std::mutex> lock( m_ );
    workersInformation_[ workerId ] = WorkerInformation( numberOfActiveConnections, outputSpeed );

    if ( !updatedInformationEvent_->isSignaled() ) {
        updatedInformationEvent_->setEvent();
    }

    return true;
}

void Table::readWorkersInformation( std::map <uint32_t, WorkerInformation>& workersInformation )
{
    std::lock_guard<std::mutex> lock( m_ );
    workersInformation.clear();
    workersInformation =  workersInformation_;
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