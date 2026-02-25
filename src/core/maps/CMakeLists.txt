add_library(pacman_maps STATIC)
add_library(Core::Maps ALIAS pacman_maps)
target_sources(pacman_maps PRIVATE MapsManager.cpp)
target_link_libraries(pacman_maps
    PUBLIC Core::Core
    PRIVATE glaze::glaze Utils::Utils
)
