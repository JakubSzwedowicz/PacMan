find_package(Utils CONFIG REQUIRED)

add_library(Phases STATIC)
add_library(Server::Phases ALIAS Phases)
target_sources(Phases PRIVATE
    LobbyPhase.cpp
    GamePhase.cpp
    AuthoritativeLogic.cpp
)
target_link_libraries(Phases
    PUBLIC Server::App Server::Network Server::AI Core::Simulation Server::Render
    PRIVATE Utils::Utils
)
