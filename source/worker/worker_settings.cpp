#include "worker_settings.hpp"

using namespace monzza::worker;

WorkerSettings::WorkerSettings() :
    id_( 0xFFFF )
{}

bool WorkerSettings::setId( const uint32_t id ) {
    id_ = id;
    return true;
}

uint32_t WorkerSettings::getId() const {
    return id_;
}

bool WorkerSettings::setDocumentRoot( std::string documentRoot ) {
    documentRoot_ = std::move( documentRoot );
    return true;
}

std::string WorkerSettings::getDocumentRoot() const {
    return documentRoot_;
}