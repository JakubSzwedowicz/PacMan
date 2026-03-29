find_package(Utils CONFIG REQUIRED)

add_library(Game STATIC)
add_library(Server::Game ALIAS Game)
target_sources(Game PRIVATE
    GameRunner.cpp
)
target_link_libraries(Game
    PUBLIC Server::Phases
    PRIVATE Utils::Utils
)
