cmake_minimum_required(VERSION 3.0.0)
project(logger VERSION 0.0.2 LANGUAGES C CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

find_package(spdlog REQUIRED)

add_library(logger src/logger.cpp)

target_link_libraries(logger 
    CONAN_PKG::spdlog 
)
target_include_directories(logger PUBLIC include)
set_target_properties(logger PROPERTIES PUBLIC_HEADER "include/logger.hpp")


install(TARGETS logger)

