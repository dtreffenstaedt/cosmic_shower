set(SIM_SOURCE_FILES "${PROJECT_SOURCE_DIR}/main.cpp"
                 "${PROJECT_SOURCE_DIR}/detector/detectorconstruction.cpp"
                 "${PROJECT_SOURCE_DIR}/actions/actioninitialization.cpp"
                 "${PROJECT_SOURCE_DIR}/actions/primarygeneratoraction.cpp"
                 "${PROJECT_SOURCE_DIR}/actions/eventaction.cpp"
                 "${PROJECT_SOURCE_DIR}/actions/steppingaction.cpp"
                 "${PROJECT_SOURCE_DIR}/configuration.cpp"
                 "${PROJECT_SOURCE_DIR}/parameters.cpp"
                 "${PROJECT_SOURCE_DIR}/recorder.cpp"
                 "${PROJECT_SOURCE_DIR}/cancelcriterion.cpp"
                 "${PROJECT_SOURCE_DIR}/core.cpp"
                 "${PROJECT_SOURCE_DIR}/physics/physicslist.cpp"
                 "${PROJECT_SOURCE_DIR}/physics/particlekiller.cpp"
                 "${PROJECT_SOURCE_DIR}/physics/decayconstructor.cpp"
                 "${PROJECT_SOURCE_DIR}/physics/stoppingconstructor.cpp"
                 "${PROJECT_SOURCE_DIR}/physics/hadroninelasticconstructor.cpp"
                 "${PROJECT_SOURCE_DIR}/physics/hadronelasticconstructor.cpp"
                 "${PROJECT_SOURCE_DIR}/detector/sensitivedetector.cpp"
                 "${PROJECT_SOURCE_DIR}/detector/detailedsensitivedetector.cpp"
)
set(SIM_HEADER_FILES "${PROJECT_INCLUDE_DIR}/detector/detectorconstruction.h"
                 "${PROJECT_INCLUDE_DIR}/actions/actioninitialization.h"
                 "${PROJECT_INCLUDE_DIR}/actions/primarygeneratoraction.h"
                 "${PROJECT_INCLUDE_DIR}/actions/eventaction.h"
                 "${PROJECT_INCLUDE_DIR}/actions/steppingaction.h"
                 "${PROJECT_INCLUDE_DIR}/configuration.h"
                 "${PROJECT_INCLUDE_DIR}/parameters.h"
                 "${PROJECT_INCLUDE_DIR}/recorder.h"
                 "${PROJECT_INCLUDE_DIR}/cancelcriterion.h"
                 "${PROJECT_INCLUDE_DIR}/core.h"
                 "${PROJECT_INCLUDE_DIR}/physics/physicslist.h"
                 "${PROJECT_INCLUDE_DIR}/physics/particlekiller.h"
                 "${PROJECT_INCLUDE_DIR}/physics/decayconstructor.h"
                 "${PROJECT_INCLUDE_DIR}/physics/stoppingconstructor.h"
                 "${PROJECT_INCLUDE_DIR}/physics/hadroninelasticconstructor.h"
                 "${PROJECT_INCLUDE_DIR}/physics/hadronelasticconstructor.h"
                 "${PROJECT_INCLUDE_DIR}/detector/sensitivedetector.h"
                 "${PROJECT_INCLUDE_DIR}/detector/detailedsensitivedetector.h"
)

if(${SHOWER_BENCHMARK})
set(SIM_SOURCE_FILES "${SIM_SOURCE_FILES}"
    "${PROJECT_SOURCE_DIR}/benchmark.cpp")
set(SIM_HEADER_FILES "${SIM_HEADER_FILES}"
    "${PROJECT_INCLUDE_DIR}/benchmark.h")
endif(${SHOWER_BENCHMARK})

set(ATMOS_HEADER_FILES "${PROJECT_INCLUDE_DIR}/atmosphere_layers.h")
set(ATMOS_SOURCE_FILES "${PROJECT_SOURCE_DIR}/atmosphere_layers.cpp")
