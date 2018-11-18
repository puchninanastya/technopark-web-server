#ifndef MONZZA_WORKER_SETTINGS_HPP
#define MONZZA_WORKER_SETTINGS_HPP

#include <cstdint>
#include <string>

namespace monzza {
    namespace worker {
        class WorkerSettings {
        public:
            explicit WorkerSettings();

            bool setId( const uint32_t id );
            uint32_t getId() const;

            bool setDocumentRoot( std::string documentRoot );
            std::string getDocumentRoot() const;
        private:
            uint32_t id_;
            std::string documentRoot_;
        };
    }
}

#endif // MONZZA_WORKER_SETTINGS_HPP