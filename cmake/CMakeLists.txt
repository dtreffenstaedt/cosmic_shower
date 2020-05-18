cmake_minimum_required (VERSION 3.5)
project (mine)

set(PROJECT_BASE_DIR ../)
set(PROJECT_SOURCE_DIR "${PROJECT_BASE_DIR}src")
set(PROJECT_INCLUDE_DIR "${PROJECT_BASE_DIR}include")
set(PROJECT_CONFIG_DIR "${PROJECT_BASE_DIR}config")
set(PROJECT_BINARY_DIR "${PROJECT_BASE_DIR}build")

include(version.cmake)
include(options.cmake)

configure_file (
    "${PROJECT_CONFIG_DIR}/global.h.in"
    "${PROJECT_INCLUDE_DIR}/global.h"
)

include(files.cmake)

include(includedirectories.cmake)
include(subdirectories.cmake)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
include(libraries.cmake)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
