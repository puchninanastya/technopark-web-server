#ifndef MONZZA_CORE_SETTINGS_HPP
#define MONZZA_CORE_SETTINGS_HPP

#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "../../external/cpl/cpl.hpp"

#define MONZZA_DEFAULT_HTTP_PORT 8080

namespace monzza {
    namespace settings {
        class Settings {
        public:
            Settings();

            bool isValid();
            bool readSettingsFromConfigFile( std::string filePath );

            std::string getConfigFilePath() const;
            uint16_t getCpuLimit() const;
            uint16_t getThreadLimit() const;
            uint16_t getListeningPort() const;
            std::string getDocumentRoot() const;

        protected:
            bool readConfigValueByName( std::string configLine,
                                        std::string configName,
                                        int32_t& configValue ) const;

            bool readConfigValueByName( std::string configLine,
                                        std::string configName,
                                        std::string& configValue ) const;

        private:
            std::string configFilePath_;
            int32_t cpuLimit_;
            int32_t threadLimit_;
            std::string documentRoot_;
            uint16_t listeningPort_;
        };
    }
}

#endif //MONZZA_CORE_HPP
