#ifndef MONZZA_TABLE_HPP
#define MONZZA_TABLE_HPP

#include <cstdint>
#include <vector>
#include <map>
#include <mutex>

#include "../../external/cpl/cpl.hpp"

namespace monzza {
    namespace table {
        class Table {
        public:
            Table();
            ~Table();

            bool updateWorkerInformation( uint32_t workerId,
                                          uint32_t numberOfActiveConnections,
                                          uint32_t outputSpeed );
            void readWorkersInformation( std::map<uint32_t, std::pair<uint32_t, uint32_t>>& workersInformation );
            void clear();

            cpl::Event* getUpdatedInformationEvent_() const;
        private:
            std::mutex m_;
            cpl::Event* updatedInformationEvent_;
            std::map<uint32_t, std::pair<uint32_t, uint32_t>> workersInformation_;
        };
    }
}

#endif // MONZZA_TABLE_HPP
