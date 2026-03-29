find_package(Utils CONFIG REQUIRED)

add_library(ServerNetwork STATIC)
add_library(Server::Network ALIAS ServerNetwork)
target_sources(ServerNetwork PRIVATE
    NetworkEventParser.cpp
    ServerNetwork.cpp
)
target_link_libraries(ServerNetwork
    PUBLIC  Core::Core Core::Network
    PRIVATE Utils::Utils
)
