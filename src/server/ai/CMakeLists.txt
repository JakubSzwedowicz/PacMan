find_package(Utils CONFIG REQUIRED)

add_library(AI STATIC)
add_library(Server::AI ALIAS AI)
target_sources(AI PRIVATE
    AISystem.cpp
    GhostBehavior.cpp
    GhostPhaseTimer.cpp
)
target_link_libraries(AI
    PUBLIC Core::ECSSystems Core::Maps
    PRIVATE Utils::Utils
)
