find_package(Utils CONFIG REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)

# Concrete screen implementations.
add_library(Screens STATIC)
add_library(Client::Screens ALIAS Screens)
target_sources(Screens PRIVATE
    ScreenFactory.cpp
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
