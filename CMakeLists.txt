cmake_minimum_required(VERSION 3.0.0)
project(logger VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

# Download automatically, you can also just copy the conan.cmake file
# if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
#   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
#   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
#                 "${CMAKE_BINARY_DIR}/conan.cmake"
#                 TLS_VERIFY ON)
# endif()

# include(${CMAKE_BINARY_DIR}/conan.cmake)
# include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

# conan_cmake_run(REQUIRES 
#                          spdlog/1.12.0@
#         GENERATORS cmake cmake_multi cmake_find_package cmake_find_package_multi cmake_paths 
#         # BUILD missing
#         # BUILD_TYPE Debug
#         BASIC_SETUP CMAKE_TARGETS
# )

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

find_package(spdlog REQUIRED)

add_library(logger logger.cpp)

target_link_libraries(logger 
    CONAN_PKG::spdlog 
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

