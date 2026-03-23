find_package(Utils CONFIG REQUIRED)

add_library(Input STATIC)
add_library(Client::Input ALIAS Input)
target_sources(Input PRIVATE InputManager.cpp)
target_link_libraries(Input
    PUBLIC Core::Core
    PRIVATE SFML::Window Utils::Utils
)
