add_library(Render STATIC)
add_library(Server::Render ALIAS Render)
target_sources(Render PRIVATE AsciiRenderer.cpp)
target_link_libraries(Render
    PUBLIC Core::ECS Core::Maps
    PRIVATE Utils::Utils
)
