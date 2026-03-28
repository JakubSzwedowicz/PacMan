find_package(Utils CONFIG REQUIRED)

add_library(ClientNetwork STATIC)
add_library(Client::Network ALIAS ClientNetwork)
target_sources(ClientNetwork PRIVATE ClientNetwork.cpp)
target_link_libraries(ClientNetwork
    PUBLIC  Core::Core
    PRIVATE unofficial::enet::enet Utils::Utils
)
