find_package(Utils CONFIG REQUIRED)

add_library(Screen STATIC)
add_library(Client::Screen ALIAS Screen)
target_sources(Screen PRIVATE ScreenManager.cpp)

# ConfigParametersContainer.h uses a relative include "ConfigParameters/ConfigParameter.h"
# that only resolves when Utils/Config/ is explicitly on the include path.
get_target_property(_utils_incs Utils::Utils INTERFACE_INCLUDE_DIRECTORIES)
foreach(_inc IN LISTS _utils_incs)
    if(IS_DIRECTORY "${_inc}/Config")
        target_include_directories(Screen PUBLIC "${_inc}/Config")
        break()
    endif()
endforeach()

target_link_libraries(Screen
    PUBLIC Core::Core SFML::Graphics Utils::Utils
)

add_subdirectory(screens)
