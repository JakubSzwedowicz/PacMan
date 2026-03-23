find_package(Utils CONFIG REQUIRED)

add_library(Screens STATIC)
add_library(Client::Screens ALIAS Screens)
target_sources(Screens PRIVATE
    MenuScreen.cpp
    GameScreen.cpp
    LobbyScreen.cpp
    LoadingScreen.cpp
    ResultsScreen.cpp
)
target_link_libraries(Screens
    PUBLIC  Client::Screen Client::Graphics Client::Network
    PRIVATE Core::Simulation ImGui-SFML::ImGui-SFML Utils::Utils
)
