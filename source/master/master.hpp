#ifndef MONZZA_MASTER_HPP
#define MONZZA_MASTER_HPP

#include <thread>

#include "../logger/log_functionality.hpp"

#include "master_settings.hpp"
#include "master_thread.hpp"

class Master : public LogFunctionality {
public:
    Master();

    bool start( Logger* logger, MasterSettings* masterSettings );
    bool stop();
private:
    std::thread*  thread_;
    MasterThread* masterThread_;
};

#endif // MONZZA_MASTER_HPP