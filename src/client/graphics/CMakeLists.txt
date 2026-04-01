find_package(Utils CONFIG REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)

add_library(Graphics STATIC)
add_library(Client::Graphics ALIAS Graphics)
target_sources(Graphics PRIVATE AppWindow.cpp Renderer.cpp)
target_link_libraries(Graphics
    PUBLIC Core::ECS Core::Maps SFML::Graphics ImGui-SFML::ImGui-SFML
    PRIVATE Utils::Utils
)
