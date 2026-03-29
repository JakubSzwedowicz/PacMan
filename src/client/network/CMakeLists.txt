find_package(Utils CONFIG REQUIRED)

add_library(ClientNetwork STATIC)
add_library(Client::Network ALIAS ClientNetwork)
target_sources(ClientNetwork PRIVATE
    ClientNetwork.cpp
    ClientNetworkEventParser.cpp
)
target_link_libraries(ClientNetwork
    PUBLIC  Core::Core Core::Network
    PRIVATE Core::Protocol Utils::Utils
)
