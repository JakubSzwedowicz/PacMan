find_package(glaze CONFIG REQUIRED)
find_package(Utils CONFIG REQUIRED)

add_library(Maps STATIC)
add_library(Core::Maps ALIAS Maps)
target_sources(Maps PRIVATE MapsManager.cpp)
target_link_libraries(Maps
    PUBLIC Core::Core
    PRIVATE glaze::glaze Utils::Utils
)
