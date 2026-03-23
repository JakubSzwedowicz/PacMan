add_subdirectory(input)
add_subdirectory(screen)
add_subdirectory(graphics)
add_subdirectory(ui)
add_subdirectory(screens)

add_executable(PacManClient)
target_sources(PacManClient PRIVATE main.cpp ClientApp.cpp)
target_link_libraries(PacManClient PRIVATE
    Client::Input
    Client::Screens
    Client::UI
    Core::Simulation
    SFML::Audio
    SFML::Network
    unofficial::enet::enet
    glaze::glaze
    Utils::Utils
)
