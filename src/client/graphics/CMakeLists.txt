find_package(Utils CONFIG REQUIRED)

add_library(Graphics STATIC)
add_library(Client::Graphics ALIAS Graphics)
target_sources(Graphics PRIVATE Window.cpp Renderer.cpp)
target_link_libraries(Graphics
    PUBLIC Core::ECS Core::Maps SFML::Graphics
    PRIVATE Utils::Utils
)
