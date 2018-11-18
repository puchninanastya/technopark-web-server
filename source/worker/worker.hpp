#ifndef MONZZA_WORKER_HPP
#define MONZZA_WORKER_HPP

#include <thread>

#include "../logger/log_functionality.hpp"
#include "../table/table.hpp"

#include "worker_thread.hpp"
#include "worker_settings.hpp"

namespace monzza {
    namespace worker {
        class Worker : public monzza::logger::LogFunctionality {
        public:
            Worker();

            bool start( monzza::logger::Logger* logger,
                        monzza::table::Table* table,
                        WorkerSettings* workerSettings );
            bool addNewConnection( cpl::TcpServerExchangeSocket* tcpServerExchangeSocket );
            bool stop();
        private:
            std::thread*  thread_;
            WorkerThread* workerThread_;
        };
    }
}

#endif // MONZZA_WORKER_HPP
