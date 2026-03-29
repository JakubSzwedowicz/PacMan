find_package(Utils CONFIG REQUIRED)
find_package(unofficial-enet CONFIG REQUIRED)

add_library(CoreNetwork STATIC)
add_library(Core::Network ALIAS CoreNetwork)
target_sources(CoreNetwork PRIVATE ENetSourceProvider.cpp)
target_link_libraries(CoreNetwork
    PUBLIC  Core::Core
    PRIVATE unofficial::enet::enet Utils::Utils
)
