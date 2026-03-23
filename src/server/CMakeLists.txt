add_executable(PacManServer)
target_sources(PacManServer PRIVATE main.cpp ServerApp.cpp)
target_link_libraries(PacManServer PRIVATE
    Core::Simulation
    unofficial::enet::enet
    glaze::glaze
    Utils::Utils
)
