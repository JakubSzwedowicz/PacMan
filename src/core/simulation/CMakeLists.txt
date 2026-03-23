add_library(Simulation STATIC)
add_library(Core::Simulation ALIAS Simulation)
target_sources(Simulation PRIVATE Simulation.cpp)
target_link_libraries(Simulation
    PUBLIC Core::ECSSystems
    PRIVATE Utils::Utils
)
