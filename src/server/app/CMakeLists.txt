find_package(Utils CONFIG REQUIRED)

add_library(App STATIC)
add_library(Server::App ALIAS App)
target_sources(App PRIVATE ServerApp.cpp)
target_link_libraries(App
    PUBLIC Core::Core Server::Network
    PRIVATE Utils::Utils
)
