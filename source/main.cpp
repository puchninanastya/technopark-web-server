#include <iostream>

#include "core/core_settings.hpp"

int main() {
    monzza::core::CoreSettings* monzzaCoreSettings = new monzza::core::CoreSettings();

    if (!monzzaCoreSettings->readSettingsFromConfigFile( "httpd.conf" )) {
        return 1;
    }

    return 0;

}
