cmake_minimum_required (VERSION 3.5)
project (cosmic_shower)

set(CMAKE_BUILD_TYPE DEBUG)
set(PROJECT_BASE_DIR ./)
set(PROJECT_SOURCE_DIR "${PROJECT_BASE_DIR}src")
set(PROJECT_INCLUDE_DIR "${PROJECT_BASE_DIR}include")
set(PROJECT_CONFIG_DIR "${PROJECT_BASE_DIR}config")
set(PROJECT_BINARY_DIR "${PROJECT_BASE_DIR}build")

include(cmake/version.cmake)

configure_file (
    "${PROJECT_CONFIG_DIR}/global.h.in"
    "${PROJECT_BASE_DIR}source/include/global.h"
)
configure_file (
    "${PROJECT_CONFIG_DIR}/shower.cfg.in"
    "${PROJECT_BASE_DIR}shower.cfg"
)
configure_file (
    "${PROJECT_CONFIG_DIR}/init_vis.mac.in"
    "${PROJECT_BASE_DIR}init_vis.mac"
)
configure_file (
    "${PROJECT_CONFIG_DIR}/vis.mac.in"
    "${PROJECT_BASE_DIR}vis.mac"
)
add_compile_options(--std=c++17 -Wall -Wextra -Wpedantic -Werror -O3)
find_package(Geant4 REQUIRED)
find_package(libconfig++ REQUIRED)

include(cmake/files.cmake)

include(${Geant4_USE_FILE})
include_directories("${PROJECT_INCLUDE_DIR}")
include_directories(${LIBCONFIG++_INCLUDE_DIRS})

find_package(Geant4 REQUIRED)

add_executable(${PROJECT_NAME} ${SIM_SOURCE_FILES} ${SIM_HEADER_FILES})
target_link_libraries(${PROJECT_NAME} ${Geant4_LIBRARIES} ${LIBCONFIG++_LIBRARIES})

add_executable(atmosphere_layers ${ATMOS_SOURCE_FILES})
target_link_libraries(atmosphere_layers ${LIBCONFIG++_LIBRARIES})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
