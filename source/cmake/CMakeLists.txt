cmake_minimum_required (VERSION 3.5)
project (cosmic_shower)

set(PROJECT_BASE_DIR ../)
set(PROJECT_SOURCE_DIR "${PROJECT_BASE_DIR}src")
set(PROJECT_INCLUDE_DIR "${PROJECT_BASE_DIR}include")
set(PROJECT_CONFIG_DIR "${PROJECT_BASE_DIR}config")
set(PROJECT_BINARY_DIR "${PROJECT_BASE_DIR}build")

include(version.cmake)

configure_file (
    "${PROJECT_CONFIG_DIR}/global.h.in"
    "${PROJECT_BASE_DIR}source/include/global.h"
)
find_package(Geant4 REQUIRED ui_all vis_all)

include(files.cmake)

include(${Geant4_USE_FILE})
include_directories("${PROJECT_INCLUDE_DIR}")

find_package(Geant4 REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${Geant4_LIBRARIES} config++)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
