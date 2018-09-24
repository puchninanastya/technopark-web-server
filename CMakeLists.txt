cmake_minimum_required(VERSION 3.8)
project(monzza)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES source/main.cpp
        source/core/core_settings.cpp
        source/core/core_settings.hpp
        external/cpl/cpl.hpp
        external/cpl/cpl.cpp
        source/core/http_parser/http_parser.cpp
        source/core/http_parser/http_parser.hpp
        source/core/http_parser/http_request.cpp
        source/core/http_parser/http_request.hpp
        source/core/http_parser/buffer.cpp
        source/core/http_parser/buffer.hpp)

add_executable(monzza ${SOURCE_FILES})