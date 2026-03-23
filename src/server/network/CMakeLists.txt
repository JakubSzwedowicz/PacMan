find_package(Utils CONFIG REQUIRED)

add_library(Network STATIC)
add_library(Server::Network ALIAS Network)
target_sources(Network PRIVATE ServerNetwork.cpp)
target_link_libraries(Network
    PUBLIC Core::Core
    PRIVATE unofficial::enet::enet Utils::Utils
)
