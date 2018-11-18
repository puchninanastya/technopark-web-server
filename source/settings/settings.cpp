#include "settings.hpp"

using namespace monzza::settings;

Settings::Settings() :
    cpuLimit_( 0 ),
    threadLimit_( 0 ),
    listeningPort_( MONZZA_DEFAULT_HTTP_PORT )
{}

bool Settings::isValid() {
    return true;
}

bool Settings::readConfigValueByName( std::string configLine,
                                      std::string configName,
                                      int32_t& configValue ) const
{
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

bool Settings::readConfigValueByName( std::string configLine,
                                      std::string configName,
                                      std::string& configValue ) const
{
    if ( configLine.find( configName ) == std::string::npos ) {
        return false;
    }

    std::stringstream configLineStream( configLine );
    std::string tmp;

    if ( configLineStream >> tmp >> configValue ) {
        return true;
    }

    return false;
}

bool Settings::readSettingsFromConfigFile( std::string filePath ) {
    std::ifstream configFile( filePath );
    if ( !configFile ) {
        std::cout << "Failed to open config file by " << filePath << " path." << std::endl;
        return false;
    }

    std::string configLine;

    while ( std::getline( configFile, configLine ) ) {
        if ( readConfigValueByName( configLine, "cpu_limit ", cpuLimit_ )       ||
             readConfigValueByName( configLine, "thread_limit ", threadLimit_ ) ||
             readConfigValueByName( configLine, "document_root ", documentRoot_ ) )
        {
            continue;
        }
        else {
            std::cout << "Wrong configuration line: " << configLine << std::endl;
            return false;
        }
    }

    configFile.close();

    std::cout << "Configuration of core settings: " << std::endl
              << "  CPU limit: " << cpuLimit_ << std::endl
              << "  Thread limit: " << threadLimit_ << std::endl
              << "  Document root: " << documentRoot_ << std::endl;

    return true;
}

std::string Settings::getConfigFilePath() const {
    return configFilePath_;
}

uint16_t Settings::getCpuLimit() const {
    return cpuLimit_;
}

uint16_t Settings::getThreadLimit() const {
    return threadLimit_;
}

uint16_t Settings::getListeningPort() const {
    return listeningPort_;
}

std::string Settings::getDocumentRoot() const {
    return documentRoot_;
}