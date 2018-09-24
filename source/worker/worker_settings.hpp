#ifndef MONZZA_WORKER_SETTINGS_HPP
#define MONZZA_WORKER_SETTINGS_HPP

#include <stdint-gcc.h>

class WorkerSettings {
public:
    explicit WorkerSettings();

    bool setId( const uint32_t id );
    uint32_t getId() const;
private:
    uint32_t id_;
};

#endif // MONZZA_WORKER_SETTINGS_HPP
