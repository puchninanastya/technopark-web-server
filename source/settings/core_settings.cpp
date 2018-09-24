#include "core_settings.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace monzza;

CoreSettings::CoreSettings() :
        cpuLimit_(0),
        threadLimit_(0),
        listeningPort_(MONZA_DEFAULT_HTTP_PORT) {}

bool CoreSettings::isValid() {
    return true;
}

bool CoreSettings::readConfigValueByName( std::string configLine, std::string configName,
                                          int& configValue ) const {
    if ( configLine.find( configName ) == std::string::npos ) {
        return false;
    }

    std::stringstream configLineStream( configLine );
    std::string tmp;

    while ( configLineStream ) {
        if ( configLineStream >> tmp >> configValue ) {
            return true;
        }
    }
    return false;
}

bool CoreSettings::readConfigValueByName( std::string configLine, std::string configName,
                                          std::string& configValue ) const {
    if ( configLine.find( configName ) == std::string::npos ) {
        return false;
    }

    std::stringstream configLineStream( configLine );
    std::string tmp;

    if ( configLineStream >> tmp >> configValue )
        return true;
    return false;
}

bool CoreSettings::readSettingsFromConfigFile( std::string filePath ) {

    std::ifstream configFile( filePath );
    if ( !configFile ) {
        // TODO: log instead of cout
        std::cout << "Failed to open config file by " << filePath << " path." << std::endl;
        return false;
    }

    std::string configLine;

    while( std::getline( configFile, configLine ) ) {
        if (readConfigValueByName( configLine, "cpu_limit ", cpuLimit_ ) ||
            readConfigValueByName( configLine, "thread_limit ", threadLimit_ ) ||
            readConfigValueByName( configLine, "document_root ", documentRoot_ )) {
            continue;
        } else {
            // TODO: log instead of cout
            std::cout << "Wrong configuration line: " << configLine << std::endl;
            return false;
        }
    }

    configFile.close();

    // TODO: log instead of cout
    std::cout << "Configuration of core settings: " << std::endl
              << "CPU limit: " << cpuLimit_ << std::endl
              << "Thread limit: " << threadLimit_ << std::endl
              << "Document root: " << documentRoot_ << std::endl;

    return true;
}

std::string CoreSettings::getConfigFilePath() const {
    return configFilePath_;
}

uint16_t CoreSettings::getCpuLimit() const {
    return cpuLimit_;
}

uint16_t CoreSettings::getThreadLimit() const {
    return threadLimit_;
}

// cpl::sockets::PortNum CoreSettings::getListeningPort() const {
uint16_t CoreSettings::getListeningPort() const {
    return listeningPort_;
}

std::string CoreSettings::getDocumentRoot() const {
    return documentRoot_;
}