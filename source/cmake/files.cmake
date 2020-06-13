set(SIM_SOURCE_FILES "${PROJECT_SOURCE_DIR}/main.cpp"
                 "${PROJECT_SOURCE_DIR}/detectorconstruction.cpp"
                 "${PROJECT_SOURCE_DIR}/actioninitialization.cpp"
                 "${PROJECT_SOURCE_DIR}/actions/runaction.cpp"
                 "${PROJECT_SOURCE_DIR}/actions/primarygeneratoraction.cpp"
                 "${PROJECT_SOURCE_DIR}/actions/eventaction.cpp"
                 "${PROJECT_SOURCE_DIR}/actions/steppingaction.cpp"
                 "${PROJECT_SOURCE_DIR}/configmanager.cpp"
)
set(SIM_HEADER_FILES "${PROJECT_INCLUDE_DIR}/detectorconstruction.h"
                 "${PROJECT_INCLUDE_DIR}/actioninitialization.h"
                 "${PROJECT_INCLUDE_DIR}/actions/runaction.h"
                 "${PROJECT_INCLUDE_DIR}/actions/primarygeneratoraction.h"
                 "${PROJECT_INCLUDE_DIR}/actions/eventaction.h"
                 "${PROJECT_INCLUDE_DIR}/actions/steppingaction.h"
                 "${PROJECT_INCLUDE_DIR}/configmanager.h"
)
set(ATMOS_SOURCE_FILES "${PROJECT_SOURCE_DIR}/atmosphere_layers.cpp")
