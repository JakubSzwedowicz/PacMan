find_package(Utils CONFIG REQUIRED)

add_library(App STATIC)
add_library(Server::App ALIAS App)
target_sources(App PRIVATE ServerApp.cpp)
target_link_libraries(App
    PUBLIC Core::Core Server::Network Server::Game
    PRIVATE Utils::Utils
)
# ConfigParametersContainer.h uses a relative include that requires Utils/Config/ on the path.
get_target_property(_utils_inc Utils::Utils INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(App PUBLIC ${_utils_inc}/Config)
