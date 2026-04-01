find_package(glaze CONFIG REQUIRED)
find_package(Utils CONFIG REQUIRED)
find_package(SFML COMPONENTS Network Graphics Window Audio System CONFIG REQUIRED)
find_package(unofficial-enet CONFIG REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)

add_subdirectory(input)
add_subdirectory(ScreenManagement)
add_subdirectory(graphics)
add_subdirectory(network)

add_executable(PacManClient)
target_sources(PacManClient PRIVATE main.cpp app/ClientApp.cpp app/ProcessSpawner.cpp)
# ConfigParametersContainer.h uses a relative include that requires Utils/Config/ on the path.
get_target_property(_utils_inc Utils::Utils INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(PacManClient PRIVATE ${_utils_inc}/Config)
target_link_libraries(PacManClient PRIVATE
    Client::Input
    Client::Network
    Client::Screens
    Client::Graphics
    Core::Simulation
    SFML::Audio
    SFML::Network
    unofficial::enet::enet
    glaze::glaze
    Utils::Utils
)
