find_package(Utils CONFIG REQUIRED)

add_library(ServerNetwork STATIC)
add_library(Server::Network ALIAS ServerNetwork)
target_sources(ServerNetwork PRIVATE ServerNetwork.cpp)
target_link_libraries(ServerNetwork
    PUBLIC Core::Core
    PRIVATE unofficial::enet::enet Utils::Utils
)
