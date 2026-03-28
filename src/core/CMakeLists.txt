find_package(Utils CONFIG REQUIRED)

add_library(Core INTERFACE)
add_library(Core::Core ALIAS Core)
target_include_directories(Core INTERFACE ${CMAKE_SOURCE_DIR}/src)

# ConfigParametersContainer.h uses a relative include "ConfigParameters/ConfigParameter.h"
# which requires Utils/Config/ to be on the include path.
get_target_property(_utils_inc Utils::Utils INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(Core INTERFACE ${_utils_inc}/Config)

add_subdirectory(maps)
add_subdirectory(ecs)
add_subdirectory(simulation)
