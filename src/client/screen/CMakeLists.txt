find_package(Utils CONFIG REQUIRED)

add_library(Screen STATIC)
add_library(Client::Screen ALIAS Screen)
target_sources(Screen PRIVATE ScreenManager.cpp)
target_link_libraries(Screen
    PUBLIC Core::Core SFML::Graphics
    PRIVATE Utils::Utils
)
