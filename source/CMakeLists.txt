cmake_minimum_required(VERSION 3.10)
project(cosmic_shower)

set(SHOWER_BUILD_LAYERS OFF)

set(SHOWER_BUILD_TIDY OFF)
if(${SHOWER_BUILD_TIDY})
  set(CMAKE_CXX_CLANG_TIDY
      clang-tidy;
      -header-filter=^global;
      -checks=-*,readability-*,bugprone-*,performace-*,clang-analyzer-*,modernize-*,hicpp-*;
      -fix)
endif(${SHOWER_BUILD_TIDY})
set(CMAKE_BUILD_TYPE Debug)
set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(PROJECT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(PROJECT_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/config")
set(PROJECT_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

set(SHOWER_BUILD_UI OFF)
set(SHOWER_BENCHMARK OFF)

include(cmake/version.cmake)

configure_file("${PROJECT_CONFIG_DIR}/global.h.in"
               "${PROJECT_INCLUDE_DIR}/global.h")
configure_file("${PROJECT_CONFIG_DIR}/shower.cfg"
               "${PROJECT_BINARY_DIR}/shower.cfg")
configure_file("${PROJECT_CONFIG_DIR}/run"
                "${PROJECT_BINARY_DIR}/run")
configure_file("${PROJECT_CONFIG_DIR}/start"
                "${PROJECT_BINARY_DIR}/start")
configure_file("${PROJECT_CONFIG_DIR}/rundata.plt"
                "${PROJECT_BINARY_DIR}/rundata.plt")
if(${SHOWER_BUILD_UI})
  find_package(Geant4 REQUIRED ui_all vis_all)
else(${SHOWER_BUILD_UI})
  find_package(Geant4 REQUIRED)
endif(${SHOWER_BUILD_UI})
#find_package(libconfig++ REQUIRED)
set(LIBCONFIG++_INCLUDE_DIRS "")
set(LIBCONFIG++_LIBRARIES "config++")

set(Geant4_CXX_FLAGS "")
set(Geant4_CXX_FLAGS_DEBUG "")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
  -Wno-c++98-compat
  -Wno-c++98-compat-bind-to-temporary-copy
  -Wno-padded
  -Wwrite-strings
  -Wpointer-arith
  -Woverloaded-virtual
  -Wall
  -Wextra
  -Wpedantic
  -Wshadow
  -Werror
  -O3
  )


set(LIBCONFIG++_LIBRARIES "config++")

include(cmake/files.cmake)

include(${Geant4_USE_FILE})
include_directories("${PROJECT_INCLUDE_DIR}")
include_directories(${LIBCONFIG++_INCLUDE_DIRS})

find_package(Geant4 REQUIRED)

add_custom_target(clangformat COMMAND clang-format -style=WebKit -i
                                      ${ALL_SOURCES_AND_HEADERS})

add_executable(${PROJECT_NAME} ${SIM_SOURCE_FILES} ${SIM_HEADER_FILES})
target_link_libraries(${PROJECT_NAME} ${Geant4_LIBRARIES}
                      ${LIBCONFIG++_LIBRARIES} pthread)

add_executable(node ${NODE_SOURCE_FILES} ${NODE_HEADER_FILES})
target_link_libraries(node ${LIBCONFIG++_LIBRARIES} pthread)

#add_executable(vis ${VIS_SOURCE_FILES} ${VIS_HEADER_FILES})
#target_link_libraries(vis ${LIBCONFIG++_LIBRARIES} gd)
if (${SHOWER_BUILD_LAYERS})
add_executable(atmosphere_layers ${ATMOS_SOURCE_FILES} ${ATMOS_HEADER_FILES})
target_link_libraries(atmosphere_layers ${LIBCONFIG++_LIBRARIES})
endif (${SHOWER_BUILD_LAYERS})
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
