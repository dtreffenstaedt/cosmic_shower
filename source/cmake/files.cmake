set(SIM_SOURCE_FILES "${PROJECT_SOURCE_DIR}/main.cpp"
                 "${PROJECT_SOURCE_DIR}/detector/detectorconstruction.cpp"
                 "${PROJECT_SOURCE_DIR}/actions/actioninitialization.cpp"
                 "${PROJECT_SOURCE_DIR}/actions/primarygeneratoraction.cpp"
                 "${PROJECT_SOURCE_DIR}/actions/eventaction.cpp"
                 "${PROJECT_SOURCE_DIR}/actions/steppingaction.cpp"
                 "${PROJECT_SOURCE_DIR}/configmanager.cpp"
                 "${PROJECT_SOURCE_DIR}/parametermanager.cpp"
                 "${PROJECT_SOURCE_DIR}/recordermanager.cpp"
                 "${PROJECT_SOURCE_DIR}/core.cpp"
                 "${PROJECT_SOURCE_DIR}/physics/physicslist.cpp"
                 "${PROJECT_SOURCE_DIR}/physics/decayconstructor.cpp"
                 "${PROJECT_SOURCE_DIR}/physics/stoppingconstructor.cpp"
                 "${PROJECT_SOURCE_DIR}/detector/detectorhit.cpp"
                 "${PROJECT_SOURCE_DIR}/detector/sensitivedetector.cpp"
)
set(SIM_HEADER_FILES "${PROJECT_INCLUDE_DIR}/detector/detectorconstruction.h"
                 "${PROJECT_INCLUDE_DIR}/actions/actioninitialization.h"
                 "${PROJECT_INCLUDE_DIR}/actions/primarygeneratoraction.h"
                 "${PROJECT_INCLUDE_DIR}/actions/eventaction.h"
                 "${PROJECT_INCLUDE_DIR}/actions/steppingaction.h"
                 "${PROJECT_INCLUDE_DIR}/configmanager.h"
                 "${PROJECT_INCLUDE_DIR}/parametermanager.h"
                 "${PROJECT_INCLUDE_DIR}/recordermanager.h"
                 "${PROJECT_INCLUDE_DIR}/core.h"
                 "${PROJECT_INCLUDE_DIR}/physics/physicslist.h"
                 "${PROJECT_INCLUDE_DIR}/physics/decayconstructor.h"
                 "${PROJECT_INCLUDE_DIR}/physics/stoppingconstructor.h"
                 "${PROJECT_INCLUDE_DIR}/detector/detectorhit.h"
                 "${PROJECT_INCLUDE_DIR}/detector/sensitivedetector.h"
)

if(${SHOWER_BENCHMARK})
set(SIM_SOURCE_FILES "${SIM_SOURCE_FILES}"
    "${PROJECT_SOURCE_DIR}/benchmarkmanager.cpp")
set(SIM_HEADER_FILES "${SIM_HEADER_FILES}"
    "${PROJECT_INCLUDE_DIR}/benchmarkmanager.h")
endif(${SHOWER_BENCHMARK})

set(ATMOS_SOURCE_FILES "${PROJECT_SOURCE_DIR}/atmosphere_layers.cpp")
