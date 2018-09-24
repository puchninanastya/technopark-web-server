#ifndef MONZZA_LOG_LEVEL_HPP
#define MONZZA_LOG_LEVEL_HPP

enum class LogLevel {
    LEVEL_0, // Critical error messages.
    LEVEL_1, // Critical error, error and notification messages.
    LEVEL_2, // Critical error, error, notification and warning messages.
    LEVEL_3	 // Critical error, error, notification, warning and debug messages.
};

#endif // MONZZA_LOG_LEVEL_HPP