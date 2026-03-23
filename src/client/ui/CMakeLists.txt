find_package(Utils CONFIG REQUIRED)

add_library(UI STATIC)
add_library(Client::UI ALIAS UI)
target_sources(UI PRIVATE UIOverlay.cpp)
target_link_libraries(UI
    PUBLIC Core::Core SFML::Graphics ImGui-SFML::ImGui-SFML
    PRIVATE Utils::Utils
)
