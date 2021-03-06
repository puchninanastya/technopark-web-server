#include "master_settings.hpp"

using namespace monzza::master;

MasterSettings::MasterSettings() :
    numberOfWorkers_( 0xFFFF )
{}

bool MasterSettings::setIpAddress( const cpl::IpAddress ipAddress ) {
    ipAddress_ = ipAddress;
    return true;
}

bool MasterSettings::setNumberOfWorkers( const uint32_t numberOfWorkers ) {
    numberOfWorkers_ = numberOfWorkers;
    return true;
}

bool MasterSettings::setMaxPendingEvents( const uint32_t maxPendingEvents ) {
    maxPendingEvents_ = maxPendingEvents;
    return true;
}

bool MasterSettings::setDocumentRoot(const std::string documentRoot) {
    documentRoot_ = documentRoot;
    return true;
}

cpl::IpAddress MasterSettings::getIpAddress() const {
    return ipAddress_;
}

uint32_t MasterSettings::getNumberOfWorkers() const {
    return numberOfWorkers_;
}

uint32_t MasterSettings::getMaxPendingEvents() const {
    return maxPendingEvents_;
}

std::string MasterSettings::getDocumentRoot() const {
    return documentRoot_;
}
