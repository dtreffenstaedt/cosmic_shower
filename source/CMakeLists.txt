cmake_minimum_required (VERSION 3.5)
project (cosmic_shower)

set(SHOWER_BUILD_TIDY OFF)
if(${SHOWER_BUILD_TIDY})
#set(CMAKE_CXX_CLANG_TIDY clang-tidy; -header-filter=.; -checks=*,-fuchsia-*;)
set(CMAKE_CXX_CLANG_TIDY clang-tidy; -header-filter=^global; -checks=-*,readability-*,bugprone-*,performace-*,clang-analyzer-*,hicpp-*; -fix)
endif(${SHOWER_BUILD_TIDY})
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)
set(CMAKE_BUILD_TYPE Debug)
set(PROJECT_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")
set(PROJECT_SOURCE_DIR "${PROJECT_BASE_DIR}src")
set(PROJECT_INCLUDE_DIR "${PROJECT_BASE_DIR}include")
set(PROJECT_CONFIG_DIR "${PROJECT_BASE_DIR}config")
set(PROJECT_BINARY_DIR "${PROJECT_BASE_DIR}../build")

set(SHOWER_BUILD_UI OFF)
set(SHOWER_BENCHMARK OFF)

include(cmake/version.cmake)

configure_file (
    "${PROJECT_CONFIG_DIR}/global.h.in"
    "${PROJECT_INCLUDE_DIR}/global.h"
)
configure_file (
    "${PROJECT_CONFIG_DIR}/shower.cfg.in"
    "${PROJECT_BINARY_DIR}/shower.cfg"
)
configure_file (
    "${PROJECT_CONFIG_DIR}/init_vis.mac.in"
    "${PROJECT_BINARY_DIR}/init_vis.mac"
)
configure_file (
    "${PROJECT_CONFIG_DIR}/vis.mac.in"
    "${PROJECT_BINARY_DIR}/vis.mac"
)
configure_file (
    "${PROJECT_CONFIG_DIR}/plot.plt.in"
    "${PROJECT_BINARY_DIR}/plot.plt"
)
configure_file (
    "${PROJECT_CONFIG_DIR}/mem.plt.in"
    "${PROJECT_BINARY_DIR}/mem.plt"
)
configure_file (
    "${PROJECT_CONFIG_DIR}/run.in"
    "${PROJECT_BINARY_DIR}/run"
)
add_compile_options(--std=c++17 -Wall -Wextra -Wpedantic -Werror -O3)
if(${SHOWER_BUILD_UI})
find_package(Geant4 REQUIRED ui_all vis_all)
else(${SHOWER_BUILD_UI})
find_package(Geant4 REQUIRED)
endif(${SHOWER_BUILD_UI})
find_package(libconfig++ REQUIRED)

include(cmake/files.cmake)

include(${Geant4_USE_FILE})
include_directories("${PROJECT_INCLUDE_DIR}")
include_directories(${LIBCONFIG++_INCLUDE_DIRS})

find_package(Geant4 REQUIRED)

add_custom_target(clangformat COMMAND clang-format -style=WebKit -i ${SIM_SOURCE_FILES} ${SIM_HEADER_FILES} ${ATMOS_SOURCE_FILES})

add_executable(${PROJECT_NAME} ${SIM_SOURCE_FILES} ${SIM_HEADER_FILES})
target_link_libraries(${PROJECT_NAME} ${Geant4_LIBRARIES} ${LIBCONFIG++_LIBRARIES} pthread)

add_executable(atmosphere_layers ${ATMOS_SOURCE_FILES})
target_link_libraries(atmosphere_layers ${LIBCONFIG++_LIBRARIES})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
