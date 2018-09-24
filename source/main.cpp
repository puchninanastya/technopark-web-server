#include <iostream>

#include "core/core_settings.hpp"
#include "core/http_parser/http_parser.hpp"

int main() {
    auto monzzaCoreSettings = new monzza::core::CoreSettings();

    if ( !monzzaCoreSettings->readSettingsFromConfigFile( "httpd.conf" ) ) {
        return 1;
    }

    uint8_t inputBuffer[] = "GET /index.php HTTP/1.1\r\nHost: example.com\r\nAccept: text/html\r\n\r\n";
    auto httpParser = new monzza::core::http::HttpParser();

    if ( !httpParser->addData( inputBuffer, sizeof inputBuffer ) ) {
        return 1;
    }

    return 0;


}
