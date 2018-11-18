#ifndef MONZZA_MASTER_SETTINGS_HPP
#define MONZZA_MASTER_SETTINGS_HPP

#include "../../external/cpl/cpl.hpp"

namespace monzza {
    namespace master {
        class MasterSettings {
        public:
            explicit MasterSettings();

            bool setIpAddress( const cpl::IpAddress ipAddress );
            bool setNumberOfWorkers( const uint32_t numberOfWorkers );
            bool setMaxPendingEvents( const uint32_t maxPendingEvents );
            bool setDocumentRoot( const std::string documentRoot );

            cpl::IpAddress getIpAddress() const;
            uint32_t getNumberOfWorkers() const;
            uint32_t getMaxPendingEvents() const;
            std::string getDocumentRoot() const;
        private:
            cpl::IpAddress ipAddress_;
            uint32_t numberOfWorkers_;
            uint32_t maxPendingEvents_;
            std::string documentRoot_;
        };
    }
}

#endif // MONZZA_MASTER_SETTINGS_HPP