#include "worker_settings.hpp"

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