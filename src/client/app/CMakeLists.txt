add_library(ClientApp STATIC)
add_library(Client::App ALIAS ClientApp)

target_sources(ClientApp PRIVATE
    ClientApp.cpp
    ProcessSpawner.cpp
)

# ConfigParametersContainer.h uses a relative include that requires Utils/Config/ on the path.
get_target_property(_utils_inc Utils::Utils INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(ClientApp PRIVATE ${_utils_inc}/Config)

target_link_libraries(ClientApp PUBLIC
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
